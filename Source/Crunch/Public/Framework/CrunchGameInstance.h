// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CrunchGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	void StartMatch();
	
private:
	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevel;
};
