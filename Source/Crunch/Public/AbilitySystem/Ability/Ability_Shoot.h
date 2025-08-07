// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CrunchGameplayAbility.h"
#include "Ability_Shoot.generated.h"

class AProjectileActor;
/**
 * 
 */
UCLASS()
class CRUNCH_API UAbility_Shoot : public UCrunchGameplayAbility
{
	GENERATED_BODY()

public:
	UAbility_Shoot();

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

private:
	UFUNCTION()
	void StartShooting(FGameplayEventData Payload);

	UFUNCTION()
	void StopShooting(FGameplayEventData Payload);

	UFUNCTION()
	void ShootProjectile(FGameplayEventData Payload);

	AActor* GetAimTargetIfValid() const;

	void FindAimTarget();

	void StartAimTargetCheckTimer();
	void StopAimTargetCheckTimer();

	bool HaveValidAimTarget() const;
	bool IsTargetInRange() const;

	void OnAimTargetDeadTagUpdate(const FGameplayTag Tag, int32 NewCount);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ShootMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	TSubclassOf<AProjectileActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	float ProjectileShootSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	float ProjectileShootRange = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	TSubclassOf<UGameplayEffect> ShootGEClass;

	UPROPERTY()
	TObjectPtr<AActor> AimTarget;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AimTargetASC;

	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	float AimTargetCheckInterval = 0.1f;

	FTimerHandle AimTargetCheckTimerHandle;
};
