// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/Ability_BlackHole.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystem/TargetActor_GroundPick.h"
#include "AbilitySystem/TargetActor_BlackHole.h"
#include "Crunch/CrunchGameplayTags.h"

void UAbility_BlackHole::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	PlayCastBlackHoleMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayCastBlackHoleMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayCastBlackHoleMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitTargetDataTask->ValidData.AddDynamic(this, &ThisClass::PlaceBlackHoleConfirmed);
	WaitTargetDataTask->Cancelled.AddDynamic(this, &ThisClass::PlaceBlackHoleCancelled);
	WaitTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor = nullptr;
	WaitTargetDataTask->BeginSpawningActor(this, TargetActorClass, TargetActor);
	if (ATargetActor_GroundPick* TA_GroundPick = Cast<ATargetActor_GroundPick>(TargetActor))
	{
		TA_GroundPick->SetDrawDebug(ShouldDrawDebug());
		TA_GroundPick->SetTargetAreaRadius(TargetAreaRadius);
		TA_GroundPick->SetTraceLineLength(TargetTraceRange);
	}
	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);
	AddAimEffect();
}

void UAbility_BlackHole::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                    bool bWasCancelled)
{
	RemoveAimEffect();
	RemoveFocusEffect();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbility_BlackHole::PlaceBlackHoleConfirmed(const FGameplayAbilityTargetDataHandle& Data)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	RemoveAimEffect();
	AddFocusEffect();

	//这里移除是因为 我们要结束这个Montage 但是不结束Ability.
	if (PlayCastBlackHoleMontageTask)
	{
		PlayCastBlackHoleMontageTask->OnBlendOut.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnCancelled.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnCompleted.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnInterrupted.RemoveAll(this);
	}

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayHoldBlackHoleMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HoleBlackHoleMontage);
		PlayHoldBlackHoleMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackHoleMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackHoleMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackHoleMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayHoldBlackHoleMontageTask->ReadyForActivation();
	}

	BlackHoleTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, BlackHoleTargetActorClass);
	BlackHoleTargetDataTask->ValidData.AddDynamic(this, &ThisClass::FinalTargetReceived);
	BlackHoleTargetDataTask->Cancelled.AddDynamic(this, &ThisClass::FinalTargetReceived);
	BlackHoleTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor = nullptr;
	BlackHoleTargetDataTask->BeginSpawningActor(this, BlackHoleTargetActorClass, TargetActor);
	if (ATargetActor_BlackHole* TA_BlackHole = Cast<ATargetActor_BlackHole>(TargetActor))
	{
		TA_BlackHole->ConfigureBlackHole(TargetAreaRadius, BlackHolePullSpeed, BlackHoleDuration, GetOwnerTeamId());
	}
	BlackHoleTargetDataTask->FinishSpawningActor(this, TargetActor);

	TargetActor->SetActorLocation(UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data, 1).ImpactPoint);
}

void UAbility_BlackHole::PlaceBlackHoleCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	K2_EndAbility();
}

void UAbility_BlackHole::FinalTargetReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(Data, FinalBlowDamageEffectClass, GetAbilityLevel());
		FVector BlowCenterLoc = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data, 1).ImpactPoint;
		PushTargetsFromLocation(Data, BlowCenterLoc, FinalBlowPushSpeed);

		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FinalBlowMontage);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->ReadyForActivation();
	}
	else
	{
		PlayMontageLocally(FinalBlowMontage);
	}

	FGameplayCueParameters CueParams;
	CueParams.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data,1).ImpactPoint;
	CueParams.RawMagnitude = TargetAreaRadius;

	//这个函数是会处理复制的GameplayCue.
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(FinalBlowCueTag, CueParams);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(CrunchGameplayTags::GameplayCue_CameraShake, CueParams);
}

void UAbility_BlackHole::AddAimEffect()
{
	AimEffectHandle = BP_ApplyGameplayEffectToOwner(AimEffectClass, GetAbilityLevel());
}

void UAbility_BlackHole::RemoveAimEffect()
{
	if (AimEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AimEffectHandle);
	}
}

void UAbility_BlackHole::AddFocusEffect()
{
	FocusEffectHandle = BP_ApplyGameplayEffectToOwner(FocusEffectClass, GetAbilityLevel());
}

void UAbility_BlackHole::RemoveFocusEffect()
{
	if (FocusEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(FocusEffectHandle);
	}
}
