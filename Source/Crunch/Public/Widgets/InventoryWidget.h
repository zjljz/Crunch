// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventoryComponent;
class UBagItemWidget;
class UWrapBox;

/**
 *  装备栏/背包 用来显示玩家已经有的道具/装备.
 */
UCLASS()
class CRUNCH_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	void OnItemAdded(const UInventoryItem* NewItem);

	void OnItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount);

	UBagItemWidget* GetNextAvailableSlot() const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> ItemList;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UBagItemWidget> ItemWidgetClass;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComp;

	UPROPERTY()
	TArray<UBagItemWidget*> ItemWidgets;

	UPROPERTY()
	TMap<FInventoryItemHandle, UBagItemWidget*> PopulatedItemEntryWidget;
};
