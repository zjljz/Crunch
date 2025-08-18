// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CrunchGameplayAbilityTypes.h"
#include "Engine/DataAsset.h"
#include "PawnData.generated.h"

class ACrunchCharacter;
/**
 * 
 */
UCLASS()
class CRUNCH_API UPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetPawnDataAssetType();

	
	FString GetCharacterDisplayName() const { return CharacterName; }
	UTexture2D* GetOrLoadCharacterIcon() const;
	TSubclassOf<ACrunchCharacter> GetOrLoadCharacterClass() const;
	TSubclassOf<UAnimInstance> GetOrLoadCharacterAnimInstance() const;
	USkeletalMesh* GetCharacterMesh() const;

	const TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>* GetCharacterAbilities() const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FString CharacterName;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> CharacterIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<ACrunchCharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<UAnimInstance> DisplayAnimClass;
};
