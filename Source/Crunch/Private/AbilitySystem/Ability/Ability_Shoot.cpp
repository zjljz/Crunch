// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/Ability_Shoot.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/CrunchAbilitySystemStatics.h"
#include "AbilitySystem/ProjectileActor.h"
#include "Crunch/CrunchGameplayTags.h"

UAbility_Shoot::UAbility_Shoot()
{
	ActivationOwnedTags.AddTag(CrunchGameplayTags::Stats_Aim);
	ActivationOwnedTags.AddTag(CrunchGameplayTags::Stats_Crosshair);
}

void UAbility_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,TEXT("GA_Shoot Input Released ! ! !"));
	K2_EndAbility();
}

void UAbility_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_WaitGameplayEvent* WaitStartShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CrunchGameplayTags::Ability_BasicAttack_Pressed);
		WaitStartShootingEvent->EventReceived.AddDynamic(this, &ThisClass::UAbility_Shoot::StartShooting);
		WaitStartShootingEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitStopShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CrunchGameplayTags::Ability_BasicAttack_Released);
		WaitStopShootingEvent->EventReceived.AddDynamic(this, &ThisClass::UAbility_Shoot::StopShooting);
		WaitStopShootingEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CrunchGameplayTags::Ability_Shoot, nullptr, false, false);
		WaitShootProjectileEvent->EventReceived.AddDynamic(this, &ThisClass::UAbility_Shoot::ShootProjectile);
		WaitShootProjectileEvent->ReadyForActivation();
	}
}

void UAbility_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                bool bWasCancelled)
{
	if (AimTargetASC)
	{
		AimTargetASC->RegisterGameplayTagEvent(CrunchGameplayTags::Stats_Dead).RemoveAll(this);
		AimTargetASC = nullptr;
	}
	
	SendLocalGameplayEvent(CrunchGameplayTags::Target_Updated, FGameplayEventData());
	
	StopShooting(FGameplayEventData());

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbility_Shoot::StartShooting(FGameplayEventData Payload)
{
	//这里因为ShootMontageTask 不是在ActivateAbility内部 PredictionKey改变了 会出现在OwningClient不播放Montage的情况,所以需要本地PlayMontage.
	//如果不使用这样的方法 我们需要创建一个NetworkSyncPoint的Task,并WaitNetSync(使用OnlyServerWait),这样会更新PredictionKey 也可以达到同样的效果.
	if (K2_HasAuthority())
	{
		UAbilityTask_PlayMontageAndWait* ShootMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
		ShootMontageTask->ReadyForActivation();
	}
	else
	{
		PlayMontageLocally(ShootMontage);
	}
	FindAimTarget();
	StartAimTargetCheckTimer();
}

void UAbility_Shoot::StopShooting(FGameplayEventData Payload)
{
	if (ShootMontage)
	{
		StopMontageAfterCurrentSection(ShootMontage);
	}
	StopAimTargetCheckTimer();
}

void UAbility_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnParam;
		SpawnParam.Owner = OwnerAvatarActor;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
		if (USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo())
		{
			TArray<FName> OutNames;
			UGameplayTagsManager::Get().SplitGameplayTagFName(Payload.EventTag, OutNames);
			if (OutNames.Num() > 0)
			{
				FName SocketName = OutNames.Last();
				SocketLocation = MeshComp->GetSocketLocation(SocketName);
			}
		}

		if (AProjectileActor* Projectile = GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, SocketLocation, OwnerAvatarActor->GetActorRotation(), SpawnParam))
		{
			Projectile->ShootProjectile(ProjectileShootSpeed, ProjectileShootRange, GetAimTargetIfValid(), GetOwnerTeamId(), MakeOutgoingGameplayEffectSpec(ShootGEClass, GetAbilityLevel()));
		}
	}
}

AActor* UAbility_Shoot::GetAimTargetIfValid() const
{
	if (HaveValidAimTarget()) return AimTarget;

	return nullptr;
}

void UAbility_Shoot::FindAimTarget()
{
	if (HaveValidAimTarget()) return;

	if (AimTargetASC)
	{
		AimTargetASC->RegisterGameplayTagEvent(CrunchGameplayTags::Stats_Dead).RemoveAll(this);
		AimTargetASC = nullptr;
	}

	AimTarget = GetAimTarget(ProjectileShootRange, ETeamAttitude::Hostile);
	if (AimTarget)
	{
		AimTargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AimTarget);
		if (AimTargetASC)
		{
			AimTargetASC->RegisterGameplayTagEvent(CrunchGameplayTags::Stats_Dead).AddUObject(this, &ThisClass::OnAimTargetDeadTagUpdate);
		}
	}

	FGameplayEventData Data;
	Data.Target = AimTarget;
	SendLocalGameplayEvent(CrunchGameplayTags::Target_Updated, Data);
}

void UAbility_Shoot::StartAimTargetCheckTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(AimTargetCheckTimerHandle, this, &ThisClass::FindAimTarget, AimTargetCheckInterval, true);
	}
}

void UAbility_Shoot::StopAimTargetCheckTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AimTargetCheckTimerHandle);
	}
}

bool UAbility_Shoot::HaveValidAimTarget() const
{
	if (!AimTarget) return false;

	if (UCrunchAbilitySystemStatics::IsActorHaveTag(AimTarget, CrunchGameplayTags::Stats_Dead)) return false;

	if (!IsTargetInRange()) return false;

	return true;
}

bool UAbility_Shoot::IsTargetInRange() const
{
	if (!AimTarget) return false;

	float Distance = FVector::Distance(AimTarget->GetActorLocation(), GetAvatarActorFromActorInfo()->GetActorLocation());
	return Distance <= ProjectileShootRange;
}

void UAbility_Shoot::OnAimTargetDeadTagUpdate(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FindAimTarget();
	}
}
