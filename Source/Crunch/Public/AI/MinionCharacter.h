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

	void SetGoal(AActor* NewGoal);

	virtual void OnRep_TeamId() override;

	virtual void OnDeath() override;
	virtual void OnReSpawn() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName BB_GoalKeyName = "Goal";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap;
};
