// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crunch/Public/AbilitySystem/CrunchGameplayAbility.h"
#include "Crunch/Public/AbilitySystem/CrunchGameplayAbilityTypes.h"
#include "Ability_GroundBlast.generated.h"

class ATargetActor_GroundPick;

/**
 * 
 */
UCLASS()
class CRUNCH_API UAbility_GroundBlast : public UCrunchGameplayAbility
{
	GENERATED_BODY()

public:

	UAbility_GroundBlast();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetCancelled(const FGameplayAbilityTargetDataHandle& Data);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> TargetingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> CastMontage;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGenericDamageEffectDef DamageEffectDef;

	UPROPERTY(EditDefaultsOnly, Category = "Cue")
	FGameplayTag BlasterCueTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TraceLineLength = 2000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	bool bDrawDebug = false;
};
