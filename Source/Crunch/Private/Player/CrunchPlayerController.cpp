// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CrunchPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Player/CrunchPlayerCharacter.h"
#include "Widgets/GameplayWidget.h"

void ACrunchPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACrunchPlayerController, TeamId);
}

void ACrunchPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnedCharacter = Cast<ACrunchPlayerCharacter>(InPawn);
	if (OwnedCharacter)
	{
		OwnedCharacter->ServerInit();
		OwnedCharacter->SetGenericTeamId(TeamId);
	}
}

void ACrunchPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	OwnedCharacter = Cast<ACrunchPlayerCharacter>(P);
	
	if (OwnedCharacter)
	{
		OwnedCharacter->ClientInit();
		SpawnGameplayWidget();
	}
}

void ACrunchPlayerController::SpawnGameplayWidget()
{
	if (!IsLocalPlayerController())
	{
		return;
	}

	GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
	GameplayWidget->AddToViewport();
}