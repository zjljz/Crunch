// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ShopItemWidget.h"
#include "Components/ListView.h"
#include "Framework/CrunchAssetManager.h"


void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InitWithShopItem(Cast<UShopItemAsset>(ListItemObject));
	ParentListView = Cast<UListView>(GetOwningListView());
}

void UShopItemWidget::OnLeftMouseButtonClicked()
{
	OnShopItemSelected.Broadcast(this);
}

void UShopItemWidget::OnRightMouseButtonClicked()
{
	OnItemBuyIssued.Broadcast(ShopItemAsset);
}

UUserWidget* UShopItemWidget::GetWidget() const
{
	UShopItemWidget* Copy = CreateWidget<UShopItemWidget>(GetOwningPlayer(), GetClass());
	Copy->CopyFromOther(this);
	return Copy;
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetInputs() const
{
	if (const FItemCollection* Collection = UCrunchAssetManager::Get().GetCombinationForItem(ShopItemAsset))
	{
		return ItemAssetsToInterfaces(Collection->GetItems());
	}

	return TArray<const ITreeNodeInterface*>{};
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetOutputs() const
{
	if (const FItemCollection* Collection = UCrunchAssetManager::Get().GetIngredientForItem(ShopItemAsset))
	{
		return ItemAssetsToInterfaces(Collection->GetItems());
	}

	return TArray<const ITreeNodeInterface*>{};
}

const UObject* UShopItemWidget::GetItemObject() const
{
	return ShopItemAsset;
}

void UShopItemWidget::CopyFromOther(const UShopItemWidget* Other)
{
	OnItemBuyIssued = Other->OnItemBuyIssued;
	OnShopItemSelected = Other->OnShopItemSelected;
	InitWithShopItem(Other->GetShopItem());
	ParentListView = Other->ParentListView;
}

void UShopItemWidget::InitWithShopItem(const UShopItemAsset* ItemAsset)
{
	ShopItemAsset = ItemAsset;
	if (!ShopItemAsset) return;

	SetItemIcon(ShopItemAsset->GetItemIcon());
	CreateToolTipWidget(ShopItemAsset);
}

TArray<const ITreeNodeInterface*> UShopItemWidget::ItemAssetsToInterfaces(const TArray<const UShopItemAsset*>& ItemAssets) const
{
	TArray<const ITreeNodeInterface*> Result;
	if (!ParentListView) return Result;

	for (auto ItemAsset : ItemAssets)
	{
		if (const UShopItemWidget* ShopItemWidget = ParentListView->GetEntryWidgetFromItem<UShopItemWidget>(ItemAsset))
		{
			Result.Add(ShopItemWidget);
		}
	}

	return Result;
}
