// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GenericTeamAgentInterface.h"
#include "CrunchGameMode.generated.h"

class AStormCore;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACrunchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACrunchGameMode();
	
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

	virtual void StartPlay() override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

private:
	FGenericTeamId GetTeamIdForController(const AController* InController);

	AActor* FindNextStartSpotForTeam(const FGenericTeamId& TeamId) const;

	AStormCore* GetStormCore() const;

	void MatchFinished(AActor* ViewTarget, int WinTeamId) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Team System")
	TMap<FGenericTeamId, FName> TeamStartSpotTapMap;
};
