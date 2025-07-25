// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crunch/Public/AbilitySystem/CrunchGameplayAbility.h"
#include "Ability_DeadExp.generated.h"

/**
 *  被动Ability, 当人物死亡时给予 击杀者以及附近队友 Experience.
 */
UCLASS()
class CRUNCH_API UAbility_DeadExp : public UCrunchGameplayAbility
{
	GENERATED_BODY()

public:
	UAbility_DeadExp();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	TArray<AActor*> GetRewardTargets() const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float RewardRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float BaseGoldReward = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float GoldRewardPerExperience = 0.05f;
	
	UPROPERTY(EditDefaultsOnly, Category= "Reward")
	float BaseExperienceReward = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float ExperienceRewardPerExperience = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float KillerRewardPercent = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	TSubclassOf<UGameplayEffect> RewardEffectClass;
};
