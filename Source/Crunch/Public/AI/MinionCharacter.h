// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CrunchCharacter.h"
#include "GameFramework/Character.h"
#include "MinionCharacter.generated.h"


UCLASS()
class CRUNCH_API AMinionCharacter : public ACrunchCharacter
{
	GENERATED_BODY()

public:
	AMinionCharacter();
	
	void GetSkinBasedOnTeamId();

	bool IsActive()const;
	void ActivateMinion();
	
	virtual void OnRep_TeamId() override;
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap;
};
