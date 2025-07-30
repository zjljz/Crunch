// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShopItemAsset.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/InventoryItemWidget.h"
#include "BagItemWidget.generated.h"

class UInventoryItem;
class UTextBlock;
/**
 * 用于人物 背包/装备栏的ItemWidget.
 */
UCLASS()
class CRUNCH_API UBagItemWidget : public UInventoryItemWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void UpdateItemWidget(const UInventoryItem* NewItem);

	bool IsEmpty() const;
	void EmptySlot();
	FORCEINLINE int GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int NewIndex) { SlotIndex = NewIndex; }
	FORCEINLINE UTexture2D* GetIconTexture() const;

	void UpdateStackCount();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TObjectPtr<UTexture2D> EmptyTexture;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CooldownCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CooldownDuration;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ManaCost;

	//生成的Item.
	UPROPERTY()
	TObjectPtr<const UInventoryItem> Item;

	//代表在 背包/装备栏 的Index;
	int SlotIndex;
};
