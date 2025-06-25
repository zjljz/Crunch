// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CrunchGameplayAbilityTypes.generated.h"


/**
 * 这是一个枚举类，用作AbilitySystem的InputId.
 */
UENUM(BlueprintType)
enum class ECrunchAbilityInputID : uint8
{
	None				UMETA(DisplayName = "None"),
	BasicAttack			UMETA(DisplayName = "BasicAttack"),
	AbilityOne			UMETA(DisplayName = "AbilityOne"),
	AbilityTwo			UMETA(DisplayName = "AbilityTwo"),
	AbilityThree		UMETA(DisplayName = "AbilityThree"),
	AbilityFour			UMETA(DisplayName = "AbilityFour"),
	AbilityFive			UMETA(DisplayName = "AbilityFive"),
	AbilitySix			UMETA(DisplayName = "AbilitySix"),
	Confirm				UMETA(DisplayName = "Confirm"),
	Cancel				UMETA(DisplayName = "Cancel")
};