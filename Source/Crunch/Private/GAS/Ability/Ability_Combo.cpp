// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Ability_Combo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Crunch/CrunchGameplayTags.h"
#include "GAS/CrunchAbilitySystemStatics.h"

UAbility_Combo::UAbility_Combo()
{
	AbilityTags.AddTag(CrunchGameplayTags::Ability_BasicAttack);
	BlockAbilitiesWithTag.AddTag(CrunchGameplayTags::Ability_BasicAttack);
}

void UAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (ComboMontage && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		//播放一个ComboMontage动画
		UAbilityTask_PlayMontageAndWait* ComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, ComboMontage, 1.0f, FName("ComboFinished"));

		ComboMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		ComboMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		ComboMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		ComboMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		ComboMontageTask->ReadyForActivation();

		//等待ComboChange事件 --- OnlyMatchExact要设置为false 来进行不精确匹配.
		FGameplayTag ComboChangeTag = FGameplayTag::RequestGameplayTag("Ability.Combo.Change");
		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent
			(this, ComboChangeTag, nullptr, false, false);
		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &ThisClass::ComboChangedEventReceived);
		WaitComboChangeEventTask->ReadyForActivation();
	}

	//等待AnimMontage->AN_SendTargetGroup发送Damage事件.
	if (HasAuthority(&ActivationInfo))
	{
		FGameplayTag ComboDamageEventTag = FGameplayTag::RequestGameplayTag("Ability.Combo.Damage");

		UAbilityTask_WaitGameplayEvent* WaitComboDamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ComboDamageEventTag);

		WaitComboDamageEventTask->EventReceived.AddDynamic(this, &ThisClass::DoDamage);
		WaitComboDamageEventTask->ReadyForActivation();
	}

	SetupWaitComboInputPress();
}

void UAbility_Combo::ComboChangedEventReceived(FGameplayEventData Payload)
{
	FGameplayTag EventTag = Payload.EventTag;

	if (EventTag.GetTagLeafName() == "End") //如果是Ability.Combo.Change.End事件，则结束当前Ability
	{
		NextComboName = NAME_None;
		return;
	}

	NextComboName = EventTag.GetTagLeafName();
}

void UAbility_Combo::SetupWaitComboInputPress()
{
	//等待玩家按下这个GA的输入键
	UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPressTask->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
	WaitInputPressTask->ReadyForActivation();
}

void UAbility_Combo::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();

	TryCommitCombo();
}

void UAbility_Combo::TryCommitCombo()
{
	// 下一个Combo是End. 超过了Combo的时间.
	if (NextComboName == NAME_None)
	{
		return;
	}

	UAnimInstance* OwnerAnimInst = GetActorInfo().SkeletalMeshComponent->GetAnimInstance();
	if (!OwnerAnimInst)
	{
		return;
	}

	//设置Montage的下一个Section.
	OwnerAnimInst->Montage_SetNextSection(OwnerAnimInst->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
}

void UAbility_Combo::DoDamage(FGameplayEventData Payload)
{
	TArray<FHitResult> Result = GetHitResultsFromSweepLocationTargetData(Payload.TargetData, TargetSweepSphereRadius);

	for (auto& Hit : Result)
	{
		ApplyGEToHitResultActor(Hit, GetComboEffectForCurrentComboName(), GetAbilityLevel());
	}
}

TSubclassOf<UGameplayEffect> UAbility_Combo::GetComboEffectForCurrentComboName() const
{
	if (UAnimInstance* OwnerAnimInst = GetActorInfo().SkeletalMeshComponent->GetAnimInstance())
	{
		FName CurrentSection = OwnerAnimInst->Montage_GetCurrentSection(ComboMontage);
		const TSubclassOf<UGameplayEffect> GEClass = ComboEffectMap.FindRef(CurrentSection);
		if (GEClass)
		{
			return GEClass;
		}
	}
	return DefaultDamageEffect;
}
