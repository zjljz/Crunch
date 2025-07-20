// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CrunchGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

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
