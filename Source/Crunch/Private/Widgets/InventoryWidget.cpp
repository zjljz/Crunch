// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryWidget.h"

#include "AbilitySystemComponent.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Inventory/InventoryComponent.h"
#include "Widgets/BagItemWidget.h"
#include "Widgets/InventoryContextMenuWidget.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		InventoryComp = OwnerPawn->GetComponentByClass<UInventoryComponent>();
		if (InventoryComp)
		{
			InventoryComp->OnItemAdd.AddUObject(this, &ThisClass::OnItemAdded);
			InventoryComp->OnItemRemove.AddUObject(this, &ThisClass::OnItemRemoved);
			InventoryComp->OnItemStackCountChanged.AddUObject(this, &ThisClass::OnItemStackCountChanged);
			int Capacity = InventoryComp->GetCapacity();

			ItemList->ClearChildren();

			CreateContextMenuWidget();

			for (int i = 0; i < Capacity; ++i)
			{
				if (UBagItemWidget* NewEmptyWidget = CreateWidget<UBagItemWidget>(GetOwningPlayer(), ItemWidgetClass))
				{
					NewEmptyWidget->SetSlotIndex(i);

					UWrapBoxSlot* WrapBoxSlot = ItemList->AddChildToWrapBox(NewEmptyWidget);
					WrapBoxSlot->SetPadding(FMargin(2.f));

					ItemWidgets.Add(NewEmptyWidget);
					NewEmptyWidget->OnBagItemDropped.AddUObject(this, &ThisClass::HandleItemDragDrop);
					NewEmptyWidget->OnLeftButtonClicked.AddUObject(InventoryComp, &UInventoryComponent::TryActivateItem);
					NewEmptyWidget->OnRightButtonClicked.AddUObject(this, &ThisClass::ToggleContextMenu);

					//@todo: 这里获取ASC 需要修改.
					OwnerPawn->GetComponentByClass<UAbilitySystemComponent>()->AbilityCommittedCallbacks.AddUObject(NewEmptyWidget, &UBagItemWidget::OnAbilityCommited);
				}
			}
		}
	}
}

void UInventoryWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	if (!NewWidgetPath.ContainsWidget(ContextMenuWidget->GetCachedWidget().Get()))
	{
		ClearContextMenu();
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

void UInventoryWidget::OnItemRemoved(const FInventoryItemHandle& Handle)
{
	if (UBagItemWidget* Widget = *PopulatedItemEntryWidget.Find(Handle))
	{
		Widget->EmptySlot();
		PopulatedItemEntryWidget.Remove(Handle);
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

void UInventoryWidget::HandleItemDragDrop(UBagItemWidget* DestinationWidget, UBagItemWidget* SourceWidget)
{
	const UInventoryItem* SourceItem = SourceWidget->GetInventoryItem();
	const UInventoryItem* DestinationItem = DestinationWidget->GetInventoryItem();

	//这里交换了俩个Widget的Item.
	DestinationWidget->UpdateItemWidget(SourceItem);
	SourceWidget->UpdateItemWidget(DestinationItem);

	//先设置DestinationWidget 因为SourceWidget一定不会是空.  但是此时DestinationItem可能为空.
	if (DestinationItem)
	{
		PopulatedItemEntryWidget[DestinationItem->GetHandle()] = SourceWidget;
	}

	if (InventoryComp)
	{
		InventoryComp->OnInventorySlotChanged(SourceWidget->GetItemHandle(), SourceWidget->GetSlotIndex());
	}

	if (!DestinationWidget->IsEmpty())
	{
		PopulatedItemEntryWidget[SourceItem->GetHandle()] = DestinationWidget;
		if (InventoryComp)
		{
			InventoryComp->OnInventorySlotChanged(DestinationWidget->GetItemHandle(), DestinationWidget->GetSlotIndex());
		}
	}
}

void UInventoryWidget::CreateContextMenuWidget()
{
	if (!ContextMenuWidgetClass) return;

	ContextMenuWidget = CreateWidget<UInventoryContextMenuWidget>(GetOwningPlayer(), ContextMenuWidgetClass);

	if (ContextMenuWidget)
	{
		ContextMenuWidget->GetSellButtonClickedEvent().AddDynamic(this, &ThisClass::SellFocusItem);
		ContextMenuWidget->GetUseButtonClickedEvent().AddDynamic(this, &ThisClass::UseFocusItem);
		ContextMenuWidget->AddToViewport(1);
		SetContextMenuWidgetVisible(false);
	}
}

void UInventoryWidget::SellFocusItem()
{
	InventoryComp->SellItem(CurFocusItemHandle);
	SetContextMenuWidgetVisible(false);
}

void UInventoryWidget::UseFocusItem()
{
	InventoryComp->TryActivateItem(CurFocusItemHandle);
	SetContextMenuWidgetVisible(false);
}

void UInventoryWidget::SetContextMenuWidgetVisible(bool bVisible)
{
	if (ContextMenuWidget)
	{
		ContextMenuWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UInventoryWidget::ToggleContextMenu(const FInventoryItemHandle& Handle)
{
	//如果点击的是当前已经点击的Handle, 或者是空Item, 那么尝试调用Clear.
	if (CurFocusItemHandle == Handle || Handle == FInventoryItemHandle::InvalidHandle())
	{
		ClearContextMenu();
		return;
	}

	CurFocusItemHandle = Handle;

	UBagItemWidget** ItemWidgetPtrPtr = PopulatedItemEntryWidget.Find(Handle);
	if (!ItemWidgetPtrPtr) return;

	UBagItemWidget* ItemWidget = *ItemWidgetPtrPtr;
	if (!ItemWidget) return;

	SetContextMenuWidgetVisible(true);
	FVector2D WidgetAbsPos = ItemWidget->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D{1.f, 0.5f});

	FVector2D ItemWidgetPixelPos, ItemWidgetViewportPos;
	USlateBlueprintLibrary::AbsoluteToViewport(this, WidgetAbsPos, ItemWidgetPixelPos, ItemWidgetViewportPos);
	if (APlayerController* PC = GetOwningPlayer())
	{
		int ViewportSizeX, ViewportSizeY;
		PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

		int OverShoot = ItemWidgetPixelPos.Y + ContextMenuWidget->GetDesiredSize().Y * Scale - ViewportSizeY;
		if (OverShoot > 0)
		{
			ItemWidgetPixelPos.Y -= OverShoot;
		}
	}
	ContextMenuWidget->SetPositionInViewport(ItemWidgetPixelPos);
}

void UInventoryWidget::ClearContextMenu()
{
	SetContextMenuWidgetVisible(false);
	CurFocusItemHandle = FInventoryItemHandle::InvalidHandle();
}
