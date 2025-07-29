// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryItemWidget.h"

#include "Components/Image.h"
#include "Widgets/ItemToolTip.h"

void UInventoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
}

void UInventoryItemWidget::SetItemIcon(UTexture2D* NewTexture)
{
	Img_ItemIcon->SetBrushFromTexture(NewTexture);
}

FReply UInventoryItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		return FReply::Handled().SetUserFocus(TakeWidget());
	}

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return FReply::Handled().SetUserFocus(TakeWidget()).DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return SuperReply;
}

FReply UInventoryItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (HasAnyUserFocus())
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			OnLeftMouseButtonClicked();
			return FReply::Handled();
		}

		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			OnRightMouseButtonClicked();
			return FReply::Handled();
		}
	}

	return SuperReply;
}

void UInventoryItemWidget::OnLeftMouseButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,TEXT("InventoryItemWidget: LeftMouseClicked."));
}

void UInventoryItemWidget::OnRightMouseButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,TEXT("InventoryItemWidget: RightMouseClicked."));
}

UItemToolTip* UInventoryItemWidget::CreateToolTipWidget(const UShopItemAsset* Item)
{
	UItemToolTip* ToolTip = nullptr;
	
	if (Item && GetOwningPlayer() && ToolTipWidgetClass)
	{
		ToolTip = CreateWidget<UItemToolTip>(GetOwningPlayer(), ToolTipWidgetClass);
		if (ToolTip)
		{
			ToolTip->SetItem(Item);
			SetToolTip(ToolTip);
		}
	}

	return ToolTip;
}
