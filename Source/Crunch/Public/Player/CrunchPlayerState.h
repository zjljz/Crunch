// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoTypes.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "CrunchPlayerState.generated.h"

class ACrunchGameState;

/**
 * 
 */
UCLASS()
class CRUNCH_API ACrunchPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACrunchPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//这个函数是把内部的部分参数 保存在传入的PlayerState中.
	virtual void CopyProperties(APlayerState* PlayerState) override;
	
protected:
	virtual void BeginPlay() override;

private:
	void OnPlayerSelectionUpdate(const TArray<FPlayerSelection>& NewSelectionArr);

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedPawnData(const UPawnData* NewPawnData);

	TSubclassOf<APawn> GetSelectedCharacterClass() const;

	FGenericTeamId GetTeamIdBasedOnSlot() const;
	
private:
	UPROPERTY(Replicated)
	FPlayerSelection PlayerSelection;

	UPROPERTY()
	TObjectPtr<ACrunchGameState> CrunchGameState;
};
