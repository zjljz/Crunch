// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventoryComponent;
class UBagItemWidget;
class UWrapBox;
class UInventoryContextMenuWidget;

/**
 *  装备栏/背包 用来显示玩家已经有的道具/装备.
 */
UCLASS()
class CRUNCH_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;

private:
	void OnItemAdded(const UInventoryItem* NewItem);

	void OnItemRemoved(const FInventoryItemHandle& Handle);

	void OnItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount);

	UBagItemWidget* GetNextAvailableSlot() const;

	//处理内部的拖动事件.
	void HandleItemDragDrop(UBagItemWidget* DestinationWidget, UBagItemWidget* SourceWidget);

	//创建右键菜单的Widget.
	void CreateContextMenuWidget();

	UFUNCTION()
	void SellFocusItem();

	UFUNCTION()
	void UseFocusItem();

	void SetContextMenuWidgetVisible(bool bVisible);

	void ToggleContextMenu(const FInventoryItemHandle& Handle);
	void ClearContextMenu();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> ItemList;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UBagItemWidget> ItemWidgetClass;

	//Item的右键菜单的WidgetClass.
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryContextMenuWidget> ContextMenuWidgetClass;

	//Item的右键菜单Widget.
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInventoryContextMenuWidget> ContextMenuWidget;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComp;

	//存储的Item对应的Widget.
	UPROPERTY()
	TArray<UBagItemWidget*> ItemWidgets;

	//Key:ItemHandle Value:存储这个Item的Widget.
	UPROPERTY()
	TMap<FInventoryItemHandle, UBagItemWidget*> PopulatedItemEntryWidget;

	FInventoryItemHandle CurFocusItemHandle;
};
