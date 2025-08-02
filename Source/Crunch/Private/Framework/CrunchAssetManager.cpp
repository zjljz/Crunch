// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CrunchAssetManager.h"

UCrunchAssetManager& UCrunchAssetManager::Get()
{
	UCrunchAssetManager* Singleton = Cast<UCrunchAssetManager>(GEngine->AssetManager);

	if (!Singleton)
	{
		UE_LOG(LogLoad, Fatal, TEXT("Asset Manager Need Set to the type CrunchAssetManager"));
		Singleton = NewObject<UCrunchAssetManager>();
	}

	return *Singleton;
}

void UCrunchAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(UShopItemAsset::GetShopItemAssetType(), {},
	                          FStreamableDelegate::CreateUObject(this, &ThisClass::OnShopItemLoadFinished, LoadFinishedCallback));
}

bool UCrunchAssetManager::GetLoadedShopItems(TArray<const UShopItemAsset*>& OutItems) const
{
	TArray<UObject*> LoadedObjects;
	bool bLoaded = GetPrimaryAssetObjectList(UShopItemAsset::GetShopItemAssetType(), LoadedObjects);

	if (bLoaded)
	{
		for (UObject* Obj : LoadedObjects)
		{
			OutItems.Add(Cast<UShopItemAsset>(Obj));
		}
	}

	return bLoaded;
}

const FItemCollection* UCrunchAssetManager::GetCombinationForItem(const UShopItemAsset* Item)
{
	return CombinationMap.Find(Item);
}

const FItemCollection* UCrunchAssetManager::GetIngredientForItem(const UShopItemAsset* Item)
{
	return IngredientMap.Find(Item);
}

void UCrunchAssetManager::OnShopItemLoadFinished(FStreamableDelegate Callback)
{
	Callback.ExecuteIfBound();
	BuildItemMaps();
}

void UCrunchAssetManager::BuildItemMaps()
{
	TArray<const UShopItemAsset*> LoadedItems;

	if (GetLoadedShopItems(LoadedItems))
	{
		for (const UShopItemAsset* Item : LoadedItems)
		{
			if (Item->GetIngredientItems().Num() == 0)
			{
				continue;
			}

			TArray<const UShopItemAsset*> Items;

			for (const TSoftObjectPtr<UShopItemAsset>& Ingredients : Item->GetIngredientItems())
			{
				UShopItemAsset* IngredientItem = Ingredients.LoadSynchronous();
				Items.Add(IngredientItem);
				AddToCombinationMap(IngredientItem, Item);
			}

			IngredientMap.Add(Item, FItemCollection{Items});
		}
	}
}

void UCrunchAssetManager::AddToCombinationMap(const UShopItemAsset* IngredientItem, const UShopItemAsset* CombinationItem)
{
	if (FItemCollection* Combinations = CombinationMap.Find(IngredientItem))
	{
		if (!CombinationMap[IngredientItem].Contain(CombinationItem))
		{
			Combinations->AddItem(CombinationItem);
		}
	}
	else
	{
		CombinationMap.Add(IngredientItem, FItemCollection{TArray<const UShopItemAsset*>{CombinationItem}});
	}
}