// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/GA_Combo.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GAS/CrunchAbilitySystemStatics.h"

UGA_Combo::UGA_Combo()
{
	AbilityTags.AddTag(UCrunchAbilitySystemStatics::GetBasicAttackAbilityTag());
	BlockAbilitiesWithTag.AddTag(UCrunchAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
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

		//等待ComboChange事件 --- 这里为OnlyMatchExact要设置为false
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

void UGA_Combo::ComboChangedEventReceived(FGameplayEventData Payload)
{
	FGameplayTag EventTag = Payload.EventTag;

	if (EventTag.GetTagLeafName() == "End") //如果是Ability.Combo.Change.End事件，则结束当前Ability
	{
		NextComboName = NAME_None;
		return;
	}

	NextComboName = EventTag.GetTagLeafName();
}

void UGA_Combo::SetupWaitComboInputPress()
{
	//等待玩家按下这个GA的输入键
	UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPressTask->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
	WaitInputPressTask->ReadyForActivation();
}

void UGA_Combo::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();

	TryCommitCombo();
}

void UGA_Combo::TryCommitCombo()
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

void UGA_Combo::DoDamage(FGameplayEventData Payload)
{
	TArray<FHitResult> Result = GetHitResultsFromSweepLocationTargetData(Payload.TargetData, 30.f, true, true);
}
