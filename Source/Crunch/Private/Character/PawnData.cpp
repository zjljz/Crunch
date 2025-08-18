// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PawnData.h"

#include "Character/CrunchCharacter.h"

FPrimaryAssetId UPawnData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetPawnDataAssetType(), GetFName());
}

FPrimaryAssetType UPawnData::GetPawnDataAssetType()
{
	return FPrimaryAssetType("PawnData");
}

UTexture2D* UPawnData::GetOrLoadCharacterIcon() const
{
	if (CharacterIcon.LoadSynchronous())
	{
		return CharacterIcon.Get();
	}

	return nullptr;
}

TSubclassOf<ACrunchCharacter> UPawnData::GetOrLoadCharacterClass() const
{
	if (CharacterClass.LoadSynchronous())
	{
		return CharacterClass.Get();
	}

	return nullptr;
}

TSubclassOf<UAnimInstance> UPawnData::GetOrLoadCharacterAnimInstance() const
{
	if (DisplayAnimClass.LoadSynchronous())
	{
		return DisplayAnimClass.Get();
	}

	return nullptr;
}

USkeletalMesh* UPawnData::GetCharacterMesh() const
{
	TSubclassOf<ACrunchCharacter> Class = GetOrLoadCharacterClass();

	if (Class && Class->GetDefaultObject<ACrunchCharacter>())
	{
		return Class->GetDefaultObject<ACrunchCharacter>()->GetMesh()->GetSkeletalMeshAsset();
	}
	
	return nullptr;
}

const TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>* UPawnData::GetCharacterAbilities() const
{
	TSubclassOf<ACrunchCharacter> Class = GetOrLoadCharacterClass();

	if (Class && Class->GetDefaultObject<ACrunchCharacter>())
	{
		return &Class->GetDefaultObject<ACrunchCharacter>()->GetAbilities();
	}

	return nullptr;
}
