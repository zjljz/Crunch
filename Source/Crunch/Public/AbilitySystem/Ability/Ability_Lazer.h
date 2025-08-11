// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CrunchGameplayAbility.h"
#include "Ability_Lazer.generated.h"

class ATargetActor_Line;
/**
 * 
 */
UCLASS()
class CRUNCH_API UAbility_Lazer : public UCrunchGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

private:
	UFUNCTION()
	void ShootLazer(FGameplayEventData Payload);

	void OnAttributeManaUpdate(const FOnAttributeChangeData& NewData);

	UFUNCTION()
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data);

private:
	UPROPERTY(EditDefaultsOnly, Category= "GameplayEffect")
	TSubclassOf<UGameplayEffect> OngoingConsumtionGEClass;

	FActiveGameplayEffectHandle OngoingConsumtionGEHandle;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect")
	TSubclassOf<UGameplayEffect> HitDamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayEffect")
	float HitPushSpeed = 3000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> LazerMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_Line> TargetActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FName VFXAttackSocketName = "LazerSocket";


	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetRange = 4000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float DetectionCylinderRadius = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingInterval = 0.3f;
};
