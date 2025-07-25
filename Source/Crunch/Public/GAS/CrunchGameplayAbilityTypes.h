// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"

#include "CrunchGameplayAbilityTypes.generated.h"


/**
 * 这是一个枚举类，用作AbilitySystem的InputId.
 */
UENUM(BlueprintType)
enum class ECrunchAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	BasicAttack UMETA(DisplayName = "BasicAttack"),
	AbilityOne UMETA(DisplayName = "AbilityOne"),
	AbilityTwo UMETA(DisplayName = "AbilityTwo"),
	AbilityThree UMETA(DisplayName = "AbilityThree"),
	AbilityFour UMETA(DisplayName = "AbilityFour"),
	AbilityFive UMETA(DisplayName = "AbilityFive"),
	AbilitySix UMETA(DisplayName = "AbilitySix"),
	Confirm UMETA(DisplayName = "Confirm"),
	Cancel UMETA(DisplayName = "Cancel")
};

//用于带有推力的GE
USTRUCT(BlueprintType)
struct FGenericDamageEffectDef
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere)
	FVector PushVelocity;
};

USTRUCT(BlueprintType)
struct FHeroBaseStats : public FTableRowBase
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> Class;

	UPROPERTY(EditDefaultsOnly)
	float Strength;

	UPROPERTY(EditDefaultsOnly)
	float Intelligence;

	//每次升级提升的Strength大小.
	UPROPERTY(EditDefaultsOnly)
	float StrengthGrowRate;

	//每次升级提升的Intelligence大小.
	UPROPERTY(EditDefaultsOnly)
	float IntelligenceGrowRate;

	UPROPERTY(EditDefaultsOnly)
	float BaseMaxHealth;

	UPROPERTY(EditDefaultsOnly)
	float BaseMaxMana;

	UPROPERTY(EditDefaultsOnly)
	float BaseAttackDamage;

	UPROPERTY(EditDefaultsOnly)
	float BaseArmor;
	
	UPROPERTY(EditDefaultsOnly)
	float BaseMoveSpeed;
};
