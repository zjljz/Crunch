// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShopItemAsset.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/InventoryItemWidget.h"
#include "ShopItemWidget.generated.h"


class UShopItemWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemBuyIssued, const UShopItemAsset*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 *  商店内商品的UI.
 */
UCLASS()
class CRUNCH_API UShopItemWidget : public UInventoryItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	FOnItemBuyIssued OnItemBuyIssued;
	FOnShopItemSelected OnShopItemSelected;
	
	//当添加Object到List中时 会创建此UI 并调用该函数 方便初始化.
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	FORCEINLINE const UShopItemAsset* GetShopItem() const { return ShopItemAsset; }

	virtual void OnLeftMouseButtonClicked() override;
	virtual void OnRightMouseButtonClicked() override;
	
private:
	UPROPERTY()
	const UShopItemAsset* ShopItemAsset;
};
