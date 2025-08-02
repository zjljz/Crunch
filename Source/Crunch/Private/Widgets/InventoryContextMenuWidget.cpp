// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryContextMenuWidget.h"
#include "Components/Button.h"

FOnButtonClickedEvent& UInventoryContextMenuWidget::GetSellButtonClickedEvent() const
{
	return Btn_Sell->OnClicked;
}

FOnButtonClickedEvent& UInventoryContextMenuWidget::GetUseButtonClickedEvent() const
{
	return Btn_Use->OnClicked;
}
