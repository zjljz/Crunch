// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CrunchGameState.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ACrunchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ACrunchGameState, PlayerSelectionArr, COND_None, REPNOTIFY_Always);
}

void ACrunchGameState::RequestPlayerSelectionChange(const APlayerState* RequestPlayer, uint8 DesiredSlot)
{
	if (!HasAuthority() || IsSlotOccupied(DesiredSlot)) return;

	FPlayerSelection* PlayerSelection = PlayerSelectionArr.FindByPredicate([&](const FPlayerSelection& Section)
	{
		return Section.IsForPlayer(RequestPlayer);
	});

	if (PlayerSelection)
	{
		PlayerSelection->SetSlot(DesiredSlot);
	}
	else
	{
		PlayerSelectionArr.Add(FPlayerSelection(DesiredSlot, RequestPlayer));
	}

	OnPlayerSelectionUpdate.Broadcast(PlayerSelectionArr);
}

void ACrunchGameState::SetCharacterSelected(const APlayerState* SelectingPlayer, const UPawnData* SelectedData)
{
	if (IsPawnDataSelected(SelectedData))
	{
		return;
	}

	FPlayerSelection* FindSelection = PlayerSelectionArr.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
	{
		return PlayerSelection.IsForPlayer(SelectingPlayer);
	});

	if (FindSelection)
	{
		FindSelection->SetSelectedPawnData(SelectedData);
		OnPlayerSelectionUpdate.Broadcast(PlayerSelectionArr);
	}
}

void ACrunchGameState::SetCharacterDeSelect(const UPawnData* PawnDataToDeSelect)
{
	if (!PawnDataToDeSelect) return;
	
	FPlayerSelection* FindSelection = PlayerSelectionArr.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
	{
		return PlayerSelection.GetSelectedPawnData() == PawnDataToDeSelect;
	});

	if (FindSelection)
	{
		FindSelection->SetSelectedPawnData(nullptr);
		OnPlayerSelectionUpdate.Broadcast(PlayerSelectionArr);
	}
}

bool ACrunchGameState::IsSlotOccupied(uint8 SlotId) const
{
	for (const auto& Selection : PlayerSelectionArr)
	{
		if (Selection.GetPlayerSlot() == SlotId)
		{
			return true;
		}
	}

	return false;
}

bool ACrunchGameState::CanStartHeroSelection() const
{
	return PlayerSelectionArr.Num() == PlayerArray.Num();
}

bool ACrunchGameState::IsPawnDataSelected(const UPawnData* PawnData) const
{
	const FPlayerSelection* FindSelection = PlayerSelectionArr.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
	{
		return PlayerSelection.GetSelectedPawnData() == PawnData;
	});

	return FindSelection != nullptr;
}

bool ACrunchGameState::CanStartMatch() const
{
	for (const FPlayerSelection& Selection : PlayerSelectionArr)
	{
		if (Selection.GetSelectedPawnData() == nullptr)
		{
			return false;
		}
	}

	return true;
}

void ACrunchGameState::OnRep_PlayerSelectionArr()
{
	OnPlayerSelectionUpdate.Broadcast(PlayerSelectionArr);
}
