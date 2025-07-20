// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "CrunchPlayerController.generated.h"

class UGameplayWidget;
class ACrunchPlayerCharacter;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACrunchPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//只在Server上调用
	virtual void OnPossess(APawn* InPawn) override;

	//在Client/Listen Server上调用
	virtual void AcknowledgePossession(APawn* P) override;

	void SpawnGameplayWidget();

private:
	UPROPERTY()
	TObjectPtr<ACrunchPlayerCharacter> OwnedCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	TObjectPtr<UGameplayWidget> GameplayWidget;

public:
	/***************************************************
	 *			Team System
	 **************************************************/

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamId = NewTeamID; }

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
};
