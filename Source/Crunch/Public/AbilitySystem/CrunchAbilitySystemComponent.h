// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CrunchGameplayAbilityTypes.h"
#include "CrunchAbilitySystemComponent.generated.h"

class UAbilitySystemGenericDataAsset;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UCrunchAbilitySystemComponent();

	//初始化 HeroAttributeSet.
	void InitBaseAttribute();

	void ServerSideInit();

	void ApplyInitialEffects();
	void GiveInitialAbilities();

	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> ApplyGE, int32 Level = 1.f);

	void ApplyFullStat();

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnManaChanged(const FOnAttributeChangeData& Data);

	const TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }

private:
	//一些通用的基础能力
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>> BaseAbilities;

	//一些专属的能力
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TObjectPtr<UAbilitySystemGenericDataAsset> DA_AbilitySystemGeneric;
};
