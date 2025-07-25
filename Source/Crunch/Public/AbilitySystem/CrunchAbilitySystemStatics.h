// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CrunchAbilitySystemStatics.generated.h"

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
};
