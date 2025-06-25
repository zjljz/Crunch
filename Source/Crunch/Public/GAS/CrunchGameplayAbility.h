// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CrunchGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                                            float SphereRadius = 30.f, bool bDrawDebug = false, bool bIgnoreSelf = true) const;
};
