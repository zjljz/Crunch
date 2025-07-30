// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BagItemWidget.h"

#include "ShopItemAsset.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/ItemToolTip.h"

void UBagItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EmptySlot();
}

void UBagItemWidget::UpdateItemWidget(const UInventoryItem* NewItem)
{
	Item = NewItem;
	if (!Item || !Item->IsValid() || Item->GetStackCount() <= 0)
	{
		EmptySlot();
		return;
	}

	SetItemIcon(NewItem->GetShopItem()->GetItemIcon());
	if (UItemToolTip* ToolTip = CreateToolTipWidget(NewItem->GetShopItem()))
	{
		ToolTip->SetPrice(NewItem->GetShopItem()->GetSellPrice());
	}

	if (Item->GetShopItem()->GetIsStackable())
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		UpdateStackCount();
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UBagItemWidget::IsEmpty() const
{
	return !Item || !Item->IsValid();
}

void UBagItemWidget::EmptySlot()
{
	Item = nullptr;

	SetItemIcon(EmptyTexture);
	SetToolTip(nullptr);

	Text_StackCount->SetVisibility(ESlateVisibility::Hidden);
	Text_CooldownCount->SetVisibility(ESlateVisibility::Hidden);
	Text_CooldownDuration->SetVisibility(ESlateVisibility::Hidden);
	Text_ManaCost->SetVisibility(ESlateVisibility::Hidden);
}

UTexture2D* UBagItemWidget::GetIconTexture() const
{
	if (Item && Item->GetShopItem())
	{
		return Item->GetShopItem()->GetItemIcon();
	}

	return nullptr;
}

void UBagItemWidget::UpdateStackCount()
{
	if (Item)
	{
		Text_StackCount->SetText(FText::AsNumber(Item->GetStackCount()));
	}
}
