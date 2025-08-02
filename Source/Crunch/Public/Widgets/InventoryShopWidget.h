// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryShopWidget.generated.h"

class UItemTreeWidget;
class UInventoryComponent;
class UShopItemAsset;
class UShopItemWidget;
class UTileView;
/**
 *  商店的UI.
 */
UCLASS()
class CRUNCH_API UInventoryShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	//让AssetManager加载对应的Asset.
	void LoadShopItems();

	//当AssetManager加载Asset完成后的回调.
	void ShopItemLoadFinished();
	
	void OnShopItemWidgetGenerated(UUserWidget& NewWidget);

	void ShowItemCombination(const UShopItemWidget* ItemWidget);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> ShopItemList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UItemTreeWidget> CombinationTree;
	
	UPROPERTY()
	TMap<const UShopItemAsset*, const UShopItemWidget*> ItemsMap;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwnerInventoryComp;
};
