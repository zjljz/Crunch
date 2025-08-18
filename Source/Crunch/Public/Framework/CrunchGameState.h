// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInfoTypes.h"
#include "CrunchGameState.generated.h"

class UPawnData;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionUpdate, const TArray<FPlayerSelection>& /* NewPlayerSelections */)


/**
 * 
 */
UCLASS()
class CRUNCH_API ACrunchGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void RequestPlayerSelectionChange(const APlayerState* RequestPlayer, uint8 DesiredSlot);

	void SetCharacterSelected(const APlayerState* SelectingPlayer, const UPawnData* SelectedData);

	void SetCharacterDeSelect(const UPawnData* PawnDataToDeSelect);
	
	bool IsSlotOccupied(uint8 SlotId) const;

	const TArray<FPlayerSelection>& GetPlayerSelectionArray() const { return PlayerSelectionArr; }

	bool CanStartHeroSelection() const;

	bool IsPawnDataSelected(const UPawnData* PawnData) const;

	bool CanStartMatch() const;
	
private:
	UFUNCTION()
	void OnRep_PlayerSelectionArr();

public:
	FOnPlayerSelectionUpdate OnPlayerSelectionUpdate;

private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerSelectionArr)
	TArray<FPlayerSelection> PlayerSelectionArr;
};
