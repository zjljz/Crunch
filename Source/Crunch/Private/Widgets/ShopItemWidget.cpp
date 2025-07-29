// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ShopItemWidget.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	ShopItemAsset = Cast<UShopItemAsset>(ListItemObject);
	if (!ShopItemAsset) return;

	SetItemIcon(ShopItemAsset->GetItemIcon());
	CreateToolTipWidget(ShopItemAsset);
}

void UShopItemWidget::OnLeftMouseButtonClicked()
{
	OnShopItemSelected.Broadcast(this);
}

void UShopItemWidget::OnRightMouseButtonClicked()
{
	OnItemBuyIssued.Broadcast(ShopItemAsset);
}
