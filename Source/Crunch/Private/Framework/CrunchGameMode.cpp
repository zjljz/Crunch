// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CrunchGameMode.h"

#include "EngineUtils.h"
#include "Framework/StormCore.h"
#include "GameFramework/PlayerStart.h"
#include "Player/CrunchPlayerController.h"
#include "Player/CrunchPlayerState.h"

APlayerController* ACrunchGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* PC = Super::SpawnPlayerController(InRemoteRole, Options);

	FGenericTeamId TeamId = GetTeamIdForController(PC);

	if (IGenericTeamAgentInterface* Interface = Cast<IGenericTeamAgentInterface>(PC))
	{
		Interface->SetGenericTeamId(TeamId);
	}

	PC->StartSpot = FindNextStartSpotForTeam(TeamId);

	return PC;
}

void ACrunchGameMode::StartPlay()
{
	Super::StartPlay();

	if (AStormCore* StormCore = GetStormCore())
	{
		StormCore->OnGoalReachedDelegate.AddUObject(this, &ThisClass::MatchFinished);
	}
}

UClass* ACrunchGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (ACrunchPlayerState* PS = InController->GetPlayerState<ACrunchPlayerState>())
	{
		if (PS->GetSelectedCharacterClass())
		{
			return PS->GetSelectedCharacterClass();
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ACrunchGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(NewPlayer);
	FGenericTeamId TeamId = GetTeamIdForController(NewPlayer);
	if (TeamAgentInterface)
	{
		TeamAgentInterface->SetGenericTeamId(TeamId);
	}
	
	if (FindNextStartSpotForTeam(TeamId))
	{
		StartSpot = FindNextStartSpotForTeam(TeamId);
		NewPlayer->StartSpot = StartSpot;
	}
	
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

FGenericTeamId ACrunchGameMode::GetTeamIdForController(const AController* InController)
{
	if (ACrunchPlayerState* PS = InController->GetPlayerState<ACrunchPlayerState>())
	{
		if (PS->GetSelectedCharacterClass())
		{
			return PS->GetTeamIdBasedOnSlot();
		}
	}

	static int PlayerCount = 0;
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2);
}

AActor* ACrunchGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamId) const
{
	const FName* StartSpotTag = TeamStartSpotTapMap.Find(TeamId);
	if (!StartSpotTag)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		if (It->PlayerStartTag == *StartSpotTag)
		{
			It->PlayerStartTag = FName("Taken");
			return *It;
		}
	}

	return nullptr;
}

AStormCore* ACrunchGameMode::GetStormCore() const
{
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AStormCore> It(World); It; ++It)
		{
			return *It;
		}
	}

	return nullptr;
}

void ACrunchGameMode::MatchFinished(AActor* ViewTarget, int WinTeamId) const
{
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<ACrunchPlayerController> It(World); It; ++It)
		{
			It->MatchFinished(ViewTarget, WinTeamId);
		}
	}
}
