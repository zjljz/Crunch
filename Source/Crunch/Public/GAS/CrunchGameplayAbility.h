// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include "CrunchGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UCrunchGameplayAbility();
	
	TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereRadius = 30.f,
	                                                            ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = false, bool bIgnoreSelf = true) const;

	//给自己一个推力.
	void PushSelf(const FVector& PushVel);

	void PushTarget(AActor* Target, const FVector& PushVel);

	void PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel);
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& PushVel);
	
	void ApplyGEToHitResultActor(const FHitResult& Hit, TSubclassOf<UGameplayEffect> EffectClass, int EffectLevel);
};
