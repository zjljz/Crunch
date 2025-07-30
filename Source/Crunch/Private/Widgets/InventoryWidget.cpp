// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryWidget.h"

#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Inventory/InventoryComponent.h"
#include "Widgets/BagItemWidget.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		InventoryComp = OwnerPawn->GetComponentByClass<UInventoryComponent>();
		if (InventoryComp)
		{
			InventoryComp->OnItemAdd.AddUObject(this, &ThisClass::OnItemAdded);
			InventoryComp->OnItemStackCountChanged.AddUObject(this, &ThisClass::OnItemStackCountChanged);
			int Capacity = InventoryComp->GetCapacity();

			ItemList->ClearChildren();

			for (int i = 0; i < Capacity; ++i)
			{
				if (UBagItemWidget* NewEmptyWidget = CreateWidget<UBagItemWidget>(GetOwningPlayer(), ItemWidgetClass))
				{
					NewEmptyWidget->SetSlotIndex(i);

					UWrapBoxSlot* WrapBoxSlot = ItemList->AddChildToWrapBox(NewEmptyWidget);
					WrapBoxSlot->SetPadding(FMargin(2.f));

					ItemWidgets.Add(NewEmptyWidget);
				}
			}
		}
	}
}

void UInventoryWidget::OnItemAdded(const UInventoryItem* NewItem)
{
	if (!NewItem) return;

	if (UBagItemWidget* AvailableWidget = GetNextAvailableSlot())
	{
		AvailableWidget->UpdateItemWidget(NewItem);
		PopulatedItemEntryWidget.Add(NewItem->GetHandle(), AvailableWidget);

		if (InventoryComp)
		{
			InventoryComp->OnInventorySlotChanged(NewItem->GetHandle(), AvailableWidget->GetSlotIndex());
		}
	}
}

void UInventoryWidget::OnItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount)
{
	if (UBagItemWidget** FoundWidget = PopulatedItemEntryWidget.Find(Handle))
	{
		(*FoundWidget)->UpdateStackCount();
	}
	
}

UBagItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
	for (UBagItemWidget* Widget : ItemWidgets)
	{
		if (Widget->IsEmpty())
		{
			return Widget;
		}
	}

	return nullptr;
}
