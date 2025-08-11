// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/Ability_Lazer.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystem/CrunchAttributeSet.h"
#include "Crunch/CrunchGameplayTags.h"
#include "AbilitySystem/TargetActor_Line.h"

void UAbility_Lazer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!K2_CommitAbility() || !LazerMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, LazerMontage);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CrunchGameplayTags::Ability_Lazer_Shoot);
		WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::ShootLazer);
		WaitEventTask->ReadyForActivation();

		UAbilityTask_WaitCancel* WaitCancelTask = UAbilityTask_WaitCancel::WaitCancel(this);
		WaitCancelTask->OnCancel.AddDynamic(this, &ThisClass::K2_EndAbility);
		WaitCancelTask->ReadyForActivation();
	}
}

void UAbility_Lazer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                bool bWasCancelled)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC && OngoingConsumtionGEHandle.IsValid())
	{
		OwnerASC->RemoveActiveGameplayEffect(OngoingConsumtionGEHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbility_Lazer::ShootLazer(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		OngoingConsumtionGEHandle = BP_ApplyGameplayEffectToOwner(OngoingConsumtionGEClass, GetAbilityLevel());
		if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
		{
			OwnerASC->GetGameplayAttributeValueChangeDelegate(UCrunchAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnAttributeManaUpdate);
		}
	}

	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, TargetActorClass);
	WaitTargetData->ValidData.AddDynamic(this, &ThisClass::OnTargetDataReceived);
	WaitTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
	if (ATargetActor_Line* TA_Line = Cast<ATargetActor_Line>(TargetActor))
	{
		TA_Line->ConfigureTargetSetting(TargetRange, DetectionCylinderRadius, TargetingInterval, GetOwnerTeamId(), ShouldDrawDebug());
	}

	WaitTargetData->FinishSpawningActor(this, TargetActor);

	if (TargetActor)
	{
		TargetActor->AttachToComponent(GetOwningComponentFromActorInfo(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, VFXAttackSocketName);
	}
}

void UAbility_Lazer::OnAttributeManaUpdate(const FOnAttributeChangeData& NewData)
{
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (!OwnerASC->CanApplyAttributeModifiers(OngoingConsumtionGEClass->GetDefaultObject<UGameplayEffect>(), GetAbilityLevel(), MakeEffectContext(CurrentSpecHandle, CurrentActorInfo)))
		{
			K2_EndAbility();
		}
	}
}

void UAbility_Lazer::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(Data, HitDamageEffectClass, GetAbilityLevel());
		PushTargets(Data, GetAvatarActorFromActorInfo()->GetActorForwardVector() * HitPushSpeed);
	}
}
