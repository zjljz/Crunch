// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CrunchGameplayAbility.h"
#include "GA_Combo.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UGA_Combo : public UCrunchGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Combo();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void ComboChangedEventReceived(FGameplayEventData Payload);

	void SetupWaitComboInputPress();
	
	UFUNCTION()
	void HandleInputPress(float TimeWaited);

	void TryCommitCombo();

	UFUNCTION()
	void DoDamage(FGameplayEventData Payload);
	
	TSubclassOf<UGameplayEffect> GetComboEffectForCurrentComboName() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ComboMontage;

	//下一个Combo的名称.
	FName NextComboName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	float TargetSweepSphereRadius = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TMap<FName, TSubclassOf<UGameplayEffect>> ComboEffectMap;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;
};
