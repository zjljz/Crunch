// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CrunchPlayerState.h"

#include "Character/PawnData.h"
#include "Framework/CrunchGameState.h"
#include "Net/UnrealNetwork.h"
#include "Character/CrunchCharacter.h"
#include "Network/CrunchNetStatics.h"

ACrunchPlayerState::ACrunchPlayerState()
{
	SetNetUpdateFrequency(100.f);
}

void ACrunchPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACrunchPlayerState, PlayerSelection);
}

void ACrunchPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (ACrunchPlayerState* InPS = Cast<ACrunchPlayerState>(PlayerState))
	{
		InPS->PlayerSelection = PlayerSelection;
	}
}

void ACrunchPlayerState::BeginPlay()
{
	Super::BeginPlay();

	CrunchGameState = Cast<ACrunchGameState>(GetWorld()->GetGameState());
	if (CrunchGameState)
	{
		CrunchGameState->OnPlayerSelectionUpdate.AddUObject(this, &ThisClass::OnPlayerSelectionUpdate);
	}
}

void ACrunchPlayerState::OnPlayerSelectionUpdate(const TArray<FPlayerSelection>& NewSelectionArr)
{
	for (auto Selection : NewSelectionArr)
	{
		if (Selection.IsForPlayer(this))
		{
			PlayerSelection = Selection;
			break;
		}
	}
}

TSubclassOf<APawn> ACrunchPlayerState::GetSelectedCharacterClass() const
{
	if (PlayerSelection.GetSelectedPawnData())
	{
		return PlayerSelection.GetSelectedPawnData()->GetOrLoadCharacterClass();
	}

	return nullptr;
}

FGenericTeamId ACrunchPlayerState::GetTeamIdBasedOnSlot() const
{
	return PlayerSelection.GetPlayerSlot() < UCrunchNetStatics::GetPlayerCountPerTeam() ? FGenericTeamId(0) : FGenericTeamId(1);
}

void ACrunchPlayerState::Server_SetSelectedPawnData_Implementation(const UPawnData* NewPawnData)
{
	if (!CrunchGameState || !NewPawnData || CrunchGameState->IsPawnDataSelected(NewPawnData)) return;

	if (PlayerSelection.GetSelectedPawnData())
	{
		CrunchGameState->SetCharacterDeSelect(PlayerSelection.GetSelectedPawnData());
	}

	PlayerSelection.SetSelectedPawnData(NewPawnData);
	CrunchGameState->SetCharacterSelected(this, NewPawnData);
}

bool ACrunchPlayerState::Server_SetSelectedPawnData_Validate(const UPawnData* NewPawnData)
{
	return true;
}
