// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CrunchAbilitySystemStatics.generated.h"

class UAbilitySystemComponent;
struct FGameplayAbilitySpec;
class UGameplayAbility;


/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchAbilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	static float GetCooldownDurationForAbility(const UGameplayAbility* Ability);

	static float GetCostForAbility(const UGameplayAbility* Ability);

	static bool IsHero(const AActor* ActorToCheck);
	static bool IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec);
	
	static bool CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec, const UAbilitySystemComponent& ASC);
	static float GetManaCostForAbility(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel);
	static float GetCooldownDurationForAbility(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel);
	static float GetCooldownRemainingForAbility(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC);
};
