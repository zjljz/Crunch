// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CrunchGameMode.h"

#include "EngineUtils.h"
#include "Framework/StormCore.h"
#include "GameFramework/PlayerStart.h"
#include "Player/CrunchPlayerController.h"

APlayerController* ACrunchGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* PC = Super::SpawnPlayerController(InRemoteRole, Options);

	FGenericTeamId TeamId = GetDefaultTeamIdForPlayerController();

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

FGenericTeamId ACrunchGameMode::GetDefaultTeamIdForPlayerController()
{
	static int PlayerCount = 0;
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2); // Alternates between team 0 and team 1
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
