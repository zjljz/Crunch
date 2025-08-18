// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MenuPlayerController.h"

#include "Blueprint/UserWidget.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);

	//考虑LS的情况.
	if (HasAuthority() && IsLocalPlayerController())
	{
		SpawnWidget();
	}
}

void AMenuPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsLocalController())
	{
		SpawnWidget();
	}
}

void AMenuPlayerController::SpawnWidget()
{
	if (!MenuWidget)
	{
		if (MenuWidgetClass)
		{
			MenuWidget = CreateWidget(this, MenuWidgetClass);
			if (MenuWidget)
			{
				MenuWidget->AddToViewport();
			}
		}
	}
}
