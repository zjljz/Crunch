// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CrunchGameplayAbility.h"
#include "Ability_Launched.generated.h"

/**
 *  一个浮空状态 类似被动的 GameplayAbility.
 */
UCLASS()
class CRUNCH_API UAbility_Launched : public UCrunchGameplayAbility
{
	GENERATED_BODY()

public:
	UAbility_Launched();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                     const FGameplayEventData* TriggerEventData) override;
};
