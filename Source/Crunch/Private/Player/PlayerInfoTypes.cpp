// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerInfoTypes.h"

#include "GameFramework/PlayerState.h"
#include "Network/CrunchNetStatics.h"

FPlayerSelection::FPlayerSelection() : Slot{GetInvalidSlot()}, PlayerUniqueId{FUniqueNetIdRepl::Invalid()}, PlayerNickName{}, SelectedPawnData(nullptr)
{
}

FPlayerSelection::FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState) : Slot{InSlot}, SelectedPawnData(nullptr)
{
	if (InPlayerState)
	{
		PlayerUniqueId = InPlayerState->GetUniqueId();
		PlayerNickName = InPlayerState->GetPlayerName();
	}
}

bool FPlayerSelection::IsForPlayer(const APlayerState* PlayerState) const
{
	if (!PlayerState) return false;

#if WITH_EDITOR
	return PlayerState->GetPlayerName() == PlayerNickName;
#else
	return PlayerState->GetUniqueId() == PlayerUniqueId;
#endif
}

bool FPlayerSelection::IsValid() const
{
#if WITH_EDITOR
	return true;
#else
	if (!PlayerUniqueId.IsValid() || Slot == GetInvalidSlot() || Slot >= UCrunchNetStatics::GetPlayerCountPerTeam() * 2) return false;
	return true;
#endif
}
