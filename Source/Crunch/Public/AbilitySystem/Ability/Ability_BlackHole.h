// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CrunchGameplayAbility.h"
#include "Ability_BlackHole.generated.h"

class ATargetActor_BlackHole;
class ATargetActor_GroundPick;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitTargetData;
/**
 * 
 */
UCLASS()
class CRUNCH_API UAbility_BlackHole : public UCrunchGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

private:
	UFUNCTION()
	void PlaceBlackHoleConfirmed(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void PlaceBlackHoleCancelled(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void FinalTargetReceived(const FGameplayAbilityTargetDataHandle& Data);
	
	void AddAimEffect();
	void RemoveAimEffect();

	void AddFocusEffect();
	void RemoveFocusEffect();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackHolePullSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackHoleDuration = 6.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> TargetingMontage;

	//用于放置BlackHoleTargetActor.
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> TargetActorClass;

	//实际用于检测Target的TA.
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_BlackHole> BlackHoleTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HoleBlackHoleMontage;
	
	UPROPERTY(EditDefaultsOnly, Category= "Targeting")
	TSubclassOf<UGameplayEffect> AimEffectClass;

	UPROPERTY(EditDefaultsOnly, Category= "Targeting")
	TSubclassOf<UGameplayEffect> FinalBlowDamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float FinalBlowPushSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> FinalBlowMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<UGameplayEffect> FocusEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FGameplayTag FinalBlowCueTag;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> PlayCastBlackHoleMontageTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitTargetData> BlackHoleTargetDataTask;
	
	FActiveGameplayEffectHandle AimEffectHandle;
	FActiveGameplayEffectHandle FocusEffectHandle;
};
