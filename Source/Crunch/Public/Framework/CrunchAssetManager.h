// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ShopItemAsset.h"
#include "CrunchAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UCrunchAssetManager& Get();

	//当调用这个函数时,会根据UShopItemAsset的AssetType来加载对应的所有PrimaryAsset,从而填补对应的Map.
	void LoadShopItems(const FStreamableDelegate& LoadFinishedCallback);

	//获取所有加载的PrimaryAsset.
	bool GetLoadedShopItems(TArray<const UShopItemAsset*>& OutItems) const;

	//获取特定Item可以合成的Items.
	const FItemCollection* GetCombinationForItem(const UShopItemAsset* Item);

	//获取合成特定Item的子Item.
	const FItemCollection* GetIngredientForItem(const UShopItemAsset* Item);
	
private:
	void OnShopItemLoadFinished(FStreamableDelegate Callback);

	void BuildItemMaps();

	/**
	 * 
	 * @param IngredientItem	可以作为合成材料的Item.
	 * @param CombinationItem	可以被IngredientItem合成的Item.
	 */
	void AddToCombinationMap(const UShopItemAsset* IngredientItem, const UShopItemAsset* CombinationItem);

	//Key: 可以用作合成材料的Item. Value: 这个Item可以合成的Items
	UPROPERTY()
	TMap<const UShopItemAsset*, FItemCollection> CombinationMap;

	//Key: 合成后的Item. Value: 用来合成的Items. 
	UPROPERTY()
	TMap<const UShopItemAsset*, FItemCollection> IngredientMap;
};
