// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopItemAsset.h"

FItemCollection::FItemCollection()
{
	
}

FItemCollection::FItemCollection(const TArray<const UShopItemAsset*>& InItems) : Items(InItems)
{
	
}

void FItemCollection::AddItem(const UShopItemAsset* NewItem, bool bAddUnique)
{
	if (bAddUnique && Contain(NewItem)) return;

	Items.Add(NewItem);
}

bool FItemCollection::Contain(const UShopItemAsset* Item) const
{
	return Items.Contains(Item);
}

FPrimaryAssetId UShopItemAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetShopItemAssetType(), GetFName());
}

FPrimaryAssetType UShopItemAsset::GetShopItemAssetType()
{
	return FPrimaryAssetType("ShopItem");
}

UTexture2D* UShopItemAsset::GetItemIcon() const
{
	return ItemSoftIcon.LoadSynchronous();
}
