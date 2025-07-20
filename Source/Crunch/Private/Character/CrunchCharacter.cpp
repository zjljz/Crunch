// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CrunchCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
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

	CrunchASC = CreateDefaultSubobject<UCrunchAbilitySystemComponent>(TEXT("CrunchASC"));
	CrunchAttributeSet = CreateDefaultSubobject<UCrunchAttributeSet>(TEXT("CrunchAttributeSet"));

	OverHeadWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidgetComp"));
	OverHeadWidgetComp->SetupAttachment(RootComponent);

	AIPerceptionStimuliSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSourceComp"));

	BindGASChangeDelegate();
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

void ACrunchCharacter::ServerInit()
{
	CrunchASC->InitAbilityActorInfo(this, this);
	CrunchASC->ApplyInitialEffects();
	CrunchASC->GiveInitialAbilities();
}

void ACrunchCharacter::ClientInit()
{
	CrunchASC->InitAbilityActorInfo(this, this);
}

void ACrunchCharacter::BindGASChangeDelegate()
{
	if (CrunchASC)
	{
		FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("Stats.Dead"));
		CrunchASC->RegisterGameplayTagEvent(DeadTag).AddUObject(this, &ThisClass::DeadTagUpDate);
	}
}

void ACrunchCharacter::DeadTagUpDate(const FGameplayTag Tag, int32 NewCount)
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
	}
}

void ACrunchCharacter::StartDeath()
{
	if (CrunchASC)
	{
		CrunchASC->CancelAllAbilities();
	}

	if (DeathMontage)
	{
		float MontageDurTime = PlayAnimMontage(DeathMontage);
		FTimerHandle Handle;
		GetWorldTimerManager().
			SetTimer(Handle, FTimerDelegate::CreateLambda
			         ([this]()
			         {
				         SetRagdollEnabled(true);
			         }),
			         MontageDurTime - TimeShiftBeforeDeathMontage, false);
	}

	SetStatusGaugeEnabled(false);

	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 当死亡时 禁用AI感知源.
	SetAIPerceptionStimuliSourceEnabled(false);

	OnDeath();
}

void ACrunchCharacter::ReSpawn()
{
	SetRagdollEnabled(false);

	SetAIPerceptionStimuliSourceEnabled(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatusGaugeEnabled(true);

	if (HasAuthority() && GetController())
	{
		SetActorTransform(GetController()->StartSpot->GetActorTransform());
	}

	if (CrunchASC)
	{
		CrunchASC->ApplyFullStat();
	}
	OnReSpawn();
}

void ACrunchCharacter::OnDeath()
{
}

void ACrunchCharacter::OnReSpawn()
{
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
