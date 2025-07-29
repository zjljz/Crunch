// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystemGenericDataAsset.generated.h"

class UGameplayEffect;
class UGameplayAbility;

/**
 *  存储一些通用的GA/GE.
 */
UCLASS()
class CRUNCH_API UAbilitySystemGenericDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	FORCEINLINE TSubclassOf<UGameplayEffect> GetFullStatEffect() const { return FullStatEffect; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDeathEffect() const { return DeathEffect; }
	FORCEINLINE const TArray<TSubclassOf<UGameplayEffect>>& GetInitialEffects() const { return InitialEffects; }
	FORCEINLINE const TArray<TSubclassOf<UGameplayAbility>>& GetPassiveAbilities() const { return PassiveAbilities; }
	FORCEINLINE const UDataTable* GetBaseStatDataTable() { return BaseStatDataTable; }
	FRealCurve* GetExpCurve() const;
private:
	//回满状态的GE
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> FullStatEffect;

	//死亡Effect
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DeathEffect;

	//用来初始化属性的GE
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	//被动GA
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;

	//基础属性的数据表.
	UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
	TObjectPtr<UDataTable> BaseStatDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Level")
	FName ExpCurveName = "ExpNeedToReachLevel";
	
	UPROPERTY(EditDefaultsOnly, Category = "Level")
	TObjectPtr<UCurveTable> ExpCurveTable;
};
