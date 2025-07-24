// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CrunchHeroAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchHeroAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchHeroAttributeSet, Intelligence);
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchHeroAttributeSet, Strength);
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchHeroAttributeSet, Experience);
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchHeroAttributeSet, PreLevelExperience);
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchHeroAttributeSet, NextLevelExperience);
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchHeroAttributeSet, Level);
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchHeroAttributeSet, MaxLevel);
	ATTRIBUTE_ACCESSORS_BASIC(UCrunchHeroAttributeSet, Gold);

private:
	UPROPERTY(ReplicatedUsing = OnRep_Intelligence)
	FGameplayAttributeData Intelligence;

	UPROPERTY(ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;

	UPROPERTY(ReplicatedUsing = OnRep_Experience)
	FGameplayAttributeData Experience;

	UPROPERTY(ReplicatedUsing = OnRep_PreLevelExperience)
	FGameplayAttributeData PreLevelExperience;

	UPROPERTY(ReplicatedUsing = OnRep_NextLevelExperience)
	FGameplayAttributeData NextLevelExperience;

	UPROPERTY(ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;

	UPROPERTY(ReplicatedUsing = OnRep_MaxLevel)
	FGameplayAttributeData MaxLevel;

	UPROPERTY(ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_PreLevelExperience(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_NextLevelExperience(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_MaxLevel(const FGameplayAttributeData& OldVal);

	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldVal);
	
};
