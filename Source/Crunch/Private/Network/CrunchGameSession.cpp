// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/CrunchGameSession.h"

#include "Framework/CrunchGameInstance.h"


bool ACrunchGameSession::ProcessAutoLogin()
{
	return true;
}

void ACrunchGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);

	if (UCrunchGameInstance* CrunchGameInst = GetGameInstance<UCrunchGameInstance>())
	{
		CrunchGameInst->OnPlayerJoined(UniqueId);
	}
}

void ACrunchGameSession::UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(InSessionName, UniqueId);

	if (UCrunchGameInstance* CrunchGameInst = GetGameInstance<UCrunchGameInstance>())
	{
		CrunchGameInst->OnPlayerLeft(UniqueId);
	}
}
