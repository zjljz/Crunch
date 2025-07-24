// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Ability_UpperCut.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Crunch/CrunchGameplayTags.h"


UAbility_UpperCut::UAbility_UpperCut()
{
	BlockAbilitiesWithTag.AddTag(CrunchGameplayTags::Ability_BasicAttack);
}

void UAbility_UpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		//先播放Montage.
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayMontageTask->ReadyForActivation();

		//等待UpperCut的Launch事件.
		FGameplayTag UpperCutLaunchTag = CrunchGameplayTags::FindTagByString("Ability.UpperCut.Launch", false);
		UAbilityTask_WaitGameplayEvent* WaitLaunchEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UpperCutLaunchTag);
		WaitLaunchEventTask->EventReceived.AddDynamic(this, &ThisClass::StartLaunching);
		WaitLaunchEventTask->ReadyForActivation();
	}
}

void UAbility_UpperCut::StartLaunching(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Payload.TargetData, TargetSweepSphereRadius,
		                                                                         ETeamAttitude::Hostile, true, true);
		PushSelf(FVector::UpVector * UpperCutLaunchSpeed); //给自己一个向上的推力.

		for (FHitResult& Hit : HitResults)
		{
			PushTarget(Hit.GetActor(), FVector::UpVector * UpperCutLaunchSpeed); //给所有命中的敌人一个向上的推力.

			ApplyGEToHitResultActor(Hit, LaunchDamageEffect, GetAbilityLevel()); //对所有命中的敌人应用伤害效果.
		}
	}

	// 监听ComboChange的Tag.
	FGameplayTag Tag = CrunchGameplayTags::FindTagByString("Ability.Combo.Change", false);
	UAbilityTask_WaitGameplayEvent* WaitComboChangeTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tag, nullptr, false, false);
	WaitComboChangeTask->EventReceived.AddDynamic(this, &ThisClass::HandleComboChangeEvent);
	WaitComboChangeTask->ReadyForActivation();

	//监听BasicAttack按键的Pressed.
	UAbilityTask_WaitGameplayEvent* WaitBasicAttackPressedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CrunchGameplayTags::Ability_BasicAttack_Pressed);
	WaitBasicAttackPressedTask->EventReceived.AddDynamic(this, &ThisClass::HandleComboCommitEvent);
	WaitBasicAttackPressedTask->ReadyForActivation();

	//监听ComboDamageTag.
	UAbilityTask_WaitGameplayEvent* WaitComboDamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, CrunchGameplayTags::Ability_Combo_Damage);
	WaitComboDamageEventTask->EventReceived.AddDynamic(this, &ThisClass::HandleComboDamageEvent);
	WaitComboDamageEventTask->ReadyForActivation();
}

void UAbility_UpperCut::HandleComboChangeEvent(FGameplayEventData Payload)
{
	FGameplayTag EventTag = Payload.EventTag;

	if (EventTag == CrunchGameplayTags::Ability_Combo_Change_End)
	{
		NextComboSectionName = NAME_None;
		return;
	}

	NextComboSectionName = EventTag.GetTagLeafName();
}

void UAbility_UpperCut::HandleComboCommitEvent(FGameplayEventData Payload)
{
	// 下一个Combo是End. 超过了Combo的时间.
	if (NextComboSectionName == NAME_None)
	{
		return;
	}

	UAnimInstance* OwnerAnimInst = GetActorInfo().SkeletalMeshComponent->GetAnimInstance();
	if (!OwnerAnimInst)
	{
		return;
	}

	//设置Montage的下一个Section.
	OwnerAnimInst->Montage_SetNextSection(OwnerAnimInst->Montage_GetCurrentSection(UpperCutMontage), NextComboSectionName, UpperCutMontage);
}

void UAbility_UpperCut::HandleComboDamageEvent(FGameplayEventData Payload)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Payload.TargetData, TargetSweepSphereRadius,
		                                                                         ETeamAttitude::Hostile, true, true);
		PushSelf(FVector::UpVector * UpperCutLaunchSpeed * 0.1f); //给自己一个向上的推力.

		const FGenericDamageEffectDef* DamageEffectDef = GetComboDamageEffectDef();
		if (!DamageEffectDef) return;
		
		for (FHitResult& Hit : HitResults)
		{
			FVector PushVel = GetAvatarActorFromActorInfo()->GetActorTransform().TransformVector(DamageEffectDef->PushVelocity);
			
			PushTarget(Hit.GetActor(), PushVel); //给所有命中的敌人一个向上的推力.

			ApplyGEToHitResultActor(Hit, DamageEffectDef->DamageEffect, GetAbilityLevel()); //对所有命中的敌人应用伤害效果.
		}
	}
}

const FGenericDamageEffectDef* UAbility_UpperCut::GetComboDamageEffectDef() const
{
	if (UAnimInstance* OwnerAnimInst = GetActorInfo().SkeletalMeshComponent->GetAnimInstance())
	{
		return ComboDamageEffectMap.Find(OwnerAnimInst->Montage_GetCurrentSection(UpperCutMontage));
	}

	return nullptr;
}
