// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryShopWidget.h"

#include "Components/TileView.h"
#include "Framework/CrunchAssetManager.h"
#include "Inventory/InventoryComponent.h"
#include "Widgets/ItemTreeWidget.h"
#include "Widgets/ShopItemWidget.h"

void UInventoryShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	LoadShopItems();
	ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &ThisClass::OnShopItemWidgetGenerated);

	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		OwnerInventoryComp = OwnerPawn->GetComponentByClass<UInventoryComponent>();
	}
}

void UInventoryShopWidget::LoadShopItems()
{
	UCrunchAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(this, &ThisClass::ShopItemLoadFinished));
}

void UInventoryShopWidget::ShopItemLoadFinished()
{
	TArray<const UShopItemAsset*> ShopItems;
	UCrunchAssetManager::Get().GetLoadedShopItems(ShopItems);

	for (const UShopItemAsset* Item : ShopItems)
	{
		ShopItemList->AddItem(const_cast<UShopItemAsset*>(Item));
	}
}

void UInventoryShopWidget::OnShopItemWidgetGenerated(UUserWidget& NewWidget)
{
	if (UShopItemWidget* ItemWidget = Cast<UShopItemWidget>(&NewWidget))
	{
		if (OwnerInventoryComp)
		{
			ItemWidget->OnItemBuyIssued.AddUObject(OwnerInventoryComp, &UInventoryComponent::TryBuySth);
		}
		ItemWidget->OnShopItemSelected.AddUObject(this, &ThisClass::ShowItemCombination);
		ItemsMap.Add(ItemWidget->GetShopItem(), ItemWidget);
	}
}

void UInventoryShopWidget::ShowItemCombination(const UShopItemWidget* ItemWidget)
{
	if (CombinationTree)
	{
		CombinationTree->DrawFromNode(ItemWidget);
	}
}
