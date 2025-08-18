// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/PlayerTeamLayoutWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Network/CrunchNetStatics.h"
#include "Widgets/Lobby/PlayerTeamSlotWidget.h"

void UPlayerTeamLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TeamOneLayoutBox->ClearChildren();
	TeamTwoLayoutBox->ClearChildren();

	if (!TeamSlotWidgetClass) return;

	for (int i = 0; i < UCrunchNetStatics::GetPlayerCountPerTeam() * 2; ++i)
	{
		UPlayerTeamSlotWidget* NewSlotWidget = CreateWidget<UPlayerTeamSlotWidget>(GetOwningPlayer(), TeamSlotWidgetClass);
		TeamSlotWidgetArray.Add(NewSlotWidget);

		UHorizontalBoxSlot* NewSlot;
		if (i < UCrunchNetStatics::GetPlayerCountPerTeam())
		{
			NewSlot = TeamOneLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
		}
		else
		{
			NewSlot = TeamTwoLayoutBox->AddChildToHorizontalBox(NewSlotWidget);
		}

		NewSlot->SetPadding(FMargin{ TeamSlotWidgetPaddingMargin });
	}
}

void UPlayerTeamLayoutWidget::UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelectionArr)
{
	for (UPlayerTeamSlotWidget* Widget : TeamSlotWidgetArray)
	{
		Widget->UpdateSlot("", nullptr);
	}

	for (const FPlayerSelection& Selection : PlayerSelectionArr)
	{
		if (!Selection.IsValid()) continue;

		TeamSlotWidgetArray[Selection.GetPlayerSlot()]->UpdateSlot(Selection.GetPlayerNickName(), Selection.GetSelectedPawnData());
	}
}
