// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoTypes.generated.h"

class UPawnData;
class APlayerState;

USTRUCT()
struct FPlayerSelection
{
	GENERATED_BODY()

public:
	FPlayerSelection();

	FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState);

	bool IsForPlayer(const APlayerState* PlayerState) const;
	bool IsValid() const;

	static uint8 GetInvalidSlot() { return uint8(255); }

	FORCEINLINE void SetSlot(uint8 InSlot) { Slot = InSlot; }
	FORCEINLINE uint8 GetPlayerSlot() const { return Slot; }
	FORCEINLINE FUniqueNetIdRepl GetPlayerUniqueId() const { return PlayerUniqueId; }
	FORCEINLINE FString GetPlayerNickName() const { return PlayerNickName; }


	FORCEINLINE const UPawnData* GetSelectedPawnData() const { return SelectedPawnData; }
	FORCEINLINE void SetSelectedPawnData(const UPawnData* InPawnData) { SelectedPawnData = InPawnData; }

private:

	//可以理解为Player的Index.
	UPROPERTY()
	uint8 Slot;

	UPROPERTY()
	FUniqueNetIdRepl PlayerUniqueId;

	UPROPERTY()
	FString PlayerNickName;

	UPROPERTY()
	TObjectPtr<const UPawnData> SelectedPawnData;
};
