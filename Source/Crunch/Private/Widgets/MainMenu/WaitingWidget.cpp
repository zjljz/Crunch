// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu/WaitingWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"


void UWaitingWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FOnButtonClickedEvent& UWaitingWidget::ClearAndGetButtonClickedEvent() const
{
	Btn_Cancel->OnClicked.Clear();
	return Btn_Cancel->OnClicked;
}

void UWaitingWidget::SetWaitInfo(const FText& InText, bool bAllowCancel)
{
	if (Text_WaitInfo && Btn_Cancel)
	{
		Btn_Cancel->SetVisibility(bAllowCancel ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		Text_WaitInfo->SetText(InText);
	}
}
