// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDisplay.generated.h"

class UPawnData;
class UCameraComponent;
/**
 *  用来在LobbyWidget中 展示所选择的Character.
 */
UCLASS()
class CRUNCH_API ACharacterDisplay : public AActor
{
	GENERATED_BODY()

public:
	ACharacterDisplay();

	void ConfigureFromPawnData(const UPawnData* InPawnData);

private:
	UPROPERTY(VisibleAnywhere, Category = "Character Display")
	TObjectPtr<USkeletalMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Character Display")
	TObjectPtr<UCameraComponent> CameraComp;
};
