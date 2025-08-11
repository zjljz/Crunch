// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CrunchPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Player/CrunchPlayerCharacter.h"
#include "Widgets/GameplayWidget.h"

class UEnhancedInputLocalPlayerSubsystem;

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

void ACrunchPlayerController::MatchFinished(AActor* ViewTarget, int WinTeamId)
{
	if (!HasAuthority()) return;
	
	OwnedCharacter->DisableInput(this);

	Client_MatchFinished(ViewTarget, WinTeamId);
}

void ACrunchPlayerController::Client_MatchFinished_Implementation(AActor* ViewTarget, int WinTeamId)
{
	SetViewTargetWithBlend(ViewTarget, MatchFinishBlendTime);
	
	FString WinLoseMsg = "You Win ! ! !";
	if (GetGenericTeamId().GetId() != WinTeamId)
	{
		WinLoseMsg = "You Lose...";
	}
	GameplayWidget->SetGameplayMenuTitle(WinLoseMsg);
	FTimerHandle ShowWinLoseStateHandle;
	GetWorldTimerManager().SetTimer(ShowWinLoseStateHandle, this, &ThisClass::ShowWinLoseState, MatchFinishBlendTime);
}

void ACrunchPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		EnhancedInputSubsystem->RemoveMappingContext(IMC_UI);
		EnhancedInputSubsystem->AddMappingContext(IMC_UI, 1);
	}

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(IA_ToggleShop, ETriggerEvent::Triggered, this, &ThisClass::ToggleShop);
		EnhancedInput->BindAction(IA_ToggleGameplayMenu, ETriggerEvent::Triggered, this, &ThisClass::ToggleGameplayMenu);
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
	GameplayWidget->ConfigureAbilities(OwnedCharacter->GetAbilities());
}

void ACrunchPlayerController::ToggleShop()
{
	if (GameplayWidget)
	{
		GameplayWidget->ToggleShopVisible();
	}
}

void ACrunchPlayerController::ToggleGameplayMenu()
{
	if (GameplayWidget)
	{
		GameplayWidget->ToggleGameplayMenu();
	}
}

void ACrunchPlayerController::ShowWinLoseState()
{
	if (GameplayWidget)
	{
		GameplayWidget->ShowGameplayMenu();
	}
}
