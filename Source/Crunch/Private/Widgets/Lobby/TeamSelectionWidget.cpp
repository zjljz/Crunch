// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/TeamSelectionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTeamSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Select->OnClicked.AddDynamic(this, &ThisClass::OnSelectButtonClicked);
}

void UTeamSelectionWidget::UpdateSlotInfo(const FString& PlayerNickName) const
{
	Text_Info->SetText(FText::FromString(PlayerNickName));
}

void UTeamSelectionWidget::OnSelectButtonClicked()
{
	OnSlotClicked.Broadcast(SlotId);
}
