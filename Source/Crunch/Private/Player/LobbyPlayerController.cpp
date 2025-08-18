// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LobbyPlayerController.h"

#include "Framework/CrunchGameInstance.h"
#include "Framework/CrunchGameState.h"
#include "GameFramework/PlayerState.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void ALobbyPlayerController::Server_RequestStartMatch_Implementation()
{
	if (UCrunchGameInstance* CrunchGameInstance = GetGameInstance<UCrunchGameInstance>())
	{
		CrunchGameInstance->StartMatch();
	}
}

bool ALobbyPlayerController::Server_RequestStartMatch_Validate()
{
	return true;
}

void ALobbyPlayerController::Server_RequestSlotSelectionChange_Implementation(uint8 NewSlotId)
{
	if (!GetWorld()) return;

	ACrunchGameState* CrunchGameState = GetWorld()->GetGameState<ACrunchGameState>();
	if (!CrunchGameState) return;

	CrunchGameState->RequestPlayerSelectionChange(GetPlayerState<APlayerState>(), NewSlotId);
}

bool ALobbyPlayerController::Server_RequestSlotSelectionChange_Validate(uint8 NewSlotId)
{
	return true;
}

void ALobbyPlayerController::Server_StartHeroSelection_Implementation()
{
	if (!HasAuthority() || !GetWorld()) return;

	for (FConstPlayerControllerIterator PCIt = GetWorld()->GetPlayerControllerIterator(); PCIt; ++PCIt)
	{
		if (ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(*PCIt))
		{
			LobbyPC->Client_StartHeroSelection();
		}
	}
}

bool ALobbyPlayerController::Server_StartHeroSelection_Validate()
{
	return true;
}

void ALobbyPlayerController::Client_StartHeroSelection_Implementation()
{
	OnSwitchToHeroSelection.ExecuteIfBound();
}
