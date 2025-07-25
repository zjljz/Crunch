// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CrunchCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Crunch/Crunch.h"
#include "Crunch/CrunchGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CrunchAbilitySystemComponent.h"
#include "GAS/CrunchAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Widgets/OverHeadStatsGauge.h"

ACrunchCharacter::ACrunchCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_SpringArm, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECollisionResponse::ECR_Ignore);

	//@todo: 这里的 ASC 对于Character 和 AICharacter 需要分情况讨论 以后需要修改创建.
	CrunchASC = CreateDefaultSubobject<UCrunchAbilitySystemComponent>(TEXT("CrunchASC"));
	CrunchAttributeSet = CreateDefaultSubobject<UCrunchAttributeSet>(TEXT("CrunchAttributeSet"));

	OverHeadWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidgetComp"));
	OverHeadWidgetComp->SetupAttachment(RootComponent);

	AIPerceptionStimuliSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSourceComp"));

	BindAbilitySystemTagChangeDelegate();
}

void ACrunchCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACrunchCharacter, TeamId);
}

void ACrunchCharacter::BeginPlay()
{
	Super::BeginPlay();

	ConfigureOverHeadWidget();

	MeshRelativeTransform = GetMesh()->GetRelativeTransform();

	AIPerceptionStimuliSourceComp->RegisterForSense(UAISense_Sight::StaticClass());
}

void ACrunchCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!NewController->IsPlayerController())
	{
		ServerInit();
	}
}


UAbilitySystemComponent* ACrunchCharacter::GetAbilitySystemComponent() const
{
	return CrunchASC;
}

void ACrunchCharacter::Server_SendGameplayEventToSelf_Implementation(FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, Payload);
}

void ACrunchCharacter::ServerInit()
{
	CrunchASC->InitAbilityActorInfo(this, this);
	CrunchASC->ServerSideInit();
}

void ACrunchCharacter::ClientInit()
{
	CrunchASC->InitAbilityActorInfo(this, this);
}

void ACrunchCharacter::BindAbilitySystemTagChangeDelegate()
{
	if (CrunchASC)
	{
		CrunchASC->RegisterGameplayTagEvent(CrunchGameplayTags::Stats_Dead).AddUObject(this, &ThisClass::OnDeadTagUpDate);
		CrunchASC->RegisterGameplayTagEvent(CrunchGameplayTags::Stats_Stun).AddUObject(this, &ThisClass::OnStunTagUpdate);
		CrunchASC->RegisterGameplayTagEvent(CrunchGameplayTags::Stats_Aim).AddUObject(this, &ThisClass::OnAimTagUpdate);

		CrunchASC->GetGameplayAttributeValueChangeDelegate(UCrunchAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ThisClass::OnAttributeMoveSpeedUpdate);
	}
}

void ACrunchCharacter::OnAttributeMoveSpeedUpdate(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void ACrunchCharacter::OnAimTagUpdate(const FGameplayTag Tag, int32 NewCount)
{
	SetIsAiming(NewCount != 0);
}

void ACrunchCharacter::OnDeadTagUpDate(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		StartDeath();
	}
	else
	{
		ReSpawn();
	}
}

void ACrunchCharacter::OnStunTagUpdate(const FGameplayTag Tag, int32 NewCount)
{
	if (IsDead()) return;

	if (NewCount != 0)
	{
		PlayAnimMontage(StunMontage);
		OnStartStun();
	}
	else
	{
		StopAnimMontage(StunMontage);
		OnEndStun();
	}
}

bool ACrunchCharacter::IsDead() const
{
	FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("Stats.Dead"));
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(DeadTag);
}

void ACrunchCharacter::RemoveDeadTag()
{
	FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("Stats.Dead"));
	GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(DeadTag));
}

void ACrunchCharacter::SetIsAiming(bool bIsAiming)
{
	bUseControllerRotationYaw = bIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bIsAiming;
	OnAimChanged(bIsAiming);
}

void ACrunchCharacter::ConfigureOverHeadWidget()
{
	if (!OverHeadWidgetComp)
	{
		return;
	}

	if (IsLocallyControlled())
	{
		OverHeadWidgetComp->SetHiddenInGame(true);
		return;
	}

	if (UOverHeadStatsGauge* OverHeadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComp->GetUserWidgetObject()))
	{
		OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());

		OverHeadWidgetComp->SetHiddenInGame(false);
		GetWorldTimerManager().ClearTimer(OverheadWidgetVisibilityTimer);
		GetWorldTimerManager().SetTimer(OverheadWidgetVisibilityTimer, this, &ACrunchCharacter::UpdateHeadGaugeVisibility,
		                                OverHeadWidgetVisibilityCheckGap, true);
	}
}

void ACrunchCharacter::UpdateHeadGaugeVisibility()
{
	if (APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		float DisToLocalPawn = FVector::DistSquared(GetActorLocation(), LocalPawn->GetActorLocation());
		OverHeadWidgetComp->SetHiddenInGame(DisToLocalPawn > OverHeadWidgetVisibilityRange);
	}
}

void ACrunchCharacter::SetStatusGaugeEnabled(bool bEnabled)
{
	GetWorldTimerManager().ClearTimer(OverheadWidgetVisibilityTimer);

	if (bEnabled)
	{
		ConfigureOverHeadWidget();
	}
	else
	{
		OverHeadWidgetComp->SetHiddenInGame(true);
	}
}


void ACrunchCharacter::SetRagdollEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ACrunchCharacter::StartDeath()
{
	//取消所有的Ability.
	if (CrunchASC)
	{
		CrunchASC->CancelAllAbilities();
	}

	//播放死亡Montage 并开启Ragdoll.
	if (DeathMontage)
	{
		float MontageDurTime = PlayAnimMontage(DeathMontage);
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda
		                                ([this]()
		                                {
			                                if (IsDead())
			                                {
				                                SetRagdollEnabled(true);
			                                }
		                                }),
		                                MontageDurTime - TimeShiftBeforeDeathMontage, false);
	}

	//关闭OverHeadUI.
	SetStatusGaugeEnabled(false);

	//GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 当死亡时 禁用AI感知源.
	SetAIPerceptionStimuliSourceEnabled(false);

	OnDeath();
}

void ACrunchCharacter::ReSpawn()
{
	SetRagdollEnabled(false);

	SetAIPerceptionStimuliSourceEnabled(true);

	//GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatusGaugeEnabled(true);

	if (CrunchASC)
	{
		CrunchASC->ApplyFullStat();
	}

	OnReSpawn();
}

void ACrunchCharacter::OnRep_TeamId()
{
}

void ACrunchCharacter::SetAIPerceptionStimuliSourceEnabled(bool bEnabled)
{
	if (!AIPerceptionStimuliSourceComp)
	{
		return;
	}

	if (bEnabled)
	{
		AIPerceptionStimuliSourceComp->RegisterWithPerceptionSystem();
	}
	else
	{
		AIPerceptionStimuliSourceComp->UnregisterFromPerceptionSystem();
	}
}
