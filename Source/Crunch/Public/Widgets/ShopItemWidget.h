// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShopItemAsset.h"
#include "TreeNodeInterface.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/InventoryItemWidget.h"
#include "ShopItemWidget.generated.h"


class UListView;
class UShopItemWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemBuyIssued, const UShopItemAsset*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 *  商店内商品的UI.
 */
UCLASS()
class CRUNCH_API UShopItemWidget : public UInventoryItemWidget, public IUserObjectListEntry, public ITreeNodeInterface
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

	/********************** ITreeNodeInterface ********************/
	virtual UUserWidget* GetWidget() const override;
	virtual TArray<const ITreeNodeInterface*> GetInputs() const override;
	virtual TArray<const ITreeNodeInterface*> GetOutputs() const override;
	virtual const UObject* GetItemObject() const override;
	/*********************** End TreeNodeInterface *****************/

private:
	void CopyFromOther(const UShopItemWidget* Other);

	//通过传入的ItemAsset 来初始化Widget.
	void InitWithShopItem(const UShopItemAsset* ItemAsset);

	TArray<const ITreeNodeInterface*> ItemAssetsToInterfaces(const TArray<const UShopItemAsset*>& ItemAssets) const;
private:

	//这个ItemWidget存在的商店UI中的ListView,存储的内容是商店中所有的Item.
	UPROPERTY()
	TObjectPtr<const UListView> ParentListView;
	
	UPROPERTY()
	TObjectPtr<const UShopItemAsset> ShopItemAsset;
};
