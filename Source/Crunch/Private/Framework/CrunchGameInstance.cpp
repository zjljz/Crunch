// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CrunchGameInstance.h"

void UCrunchGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == NM_DedicatedServer || GetWorld()->GetNetMode() == NM_ListenServer)
	{
		LoadLevelAndListen(GameLevel);
	}
}

void UCrunchGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(FPackageName::ObjectPathToObjectName(Level.ToString()));

	if (LevelURL != "")
	{
		GetWorld()->ServerTravel(LevelURL.ToString() + "?Listen");
	}
}
