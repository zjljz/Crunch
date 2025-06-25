// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CrunchPlayerController.generated.h"

class UGameplayWidget;
class ACrunchPlayerCharacter;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACrunchPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//只在Server上调用
	virtual void OnPossess(APawn* InPawn) override;

	//在Client/Listen Server上调用
	virtual void AcknowledgePossession(APawn* P) override;
	
	void SpawnGameplayWidget();
private:

	UPROPERTY()
	TObjectPtr<ACrunchPlayerCharacter> OwnedCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGameplayWidget> GameplayWidget;
};
