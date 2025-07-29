// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemToolTip.h"

#include "ShopItemAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemToolTip::SetItem(const UShopItemAsset* Item)
{
	Text_ItemName->SetText(Item->GetItemName());
	Text_ItemDescription->SetText(Item->GetItemDescription());
	Text_ItemPrice->SetText(FText::AsNumber((int)Item->GetPrice()));
	Img_ItemIcon->SetBrushFromTexture(Item->GetItemIcon());
}

void UItemToolTip::SetPrice(float NewPrice)
{
	Text_ItemPrice->SetText(FText::AsNumber((int)NewPrice));
}
