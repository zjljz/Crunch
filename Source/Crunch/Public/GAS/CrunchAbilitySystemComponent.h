// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CrunchGameplayAbilityTypes.h"
#include "CrunchAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UCrunchAbilitySystemComponent();
	
	void ApplyInitialEffects();
	void GiveInitialAbilities();

	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> ApplyGE,int32 Level = 1.f);
	
	void ApplyFullStat();
	
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	
private:

	//用来初始化属性的GE
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> FullStatEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DeathEffect;
	
	//一些通用的基础能力
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>> BaseAbilities;

	//一些专属的能力
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>  Abilities;
};
