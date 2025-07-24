// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Ability_Launched.h"

#include "Crunch/CrunchGameplayTags.h"

UAbility_Launched::UAbility_Launched()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	ActivationBlockedTags.RemoveTag(CrunchGameplayTags::Stats_Stun);
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = CrunchGameplayTags::Ability_Passive_Launch_Activate;
	AbilityTriggers.Add(TriggerData);
}

void UAbility_Launched::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		return;
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		PushSelf(TriggerEventData->TargetData.Get(0)->GetHitResult()->ImpactNormal);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}
