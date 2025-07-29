// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShopItemAsset.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UShopItemAsset;

USTRUCT()
struct FItemCollection
{
	GENERATED_BODY()

public:
	FItemCollection();

	FItemCollection(const TArray<const UShopItemAsset*>& InItems);

	void AddItem(const UShopItemAsset* NewItem, bool bAddUnique = false);
	bool Contain(const UShopItemAsset* Item) const;
	const TArray<const UShopItemAsset*>& GetItems() const { return Items; }

private:
	UPROPERTY()
	TArray<const UShopItemAsset*> Items;
};

/**
 * 
 */
UCLASS()
class CRUNCH_API UShopItemAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetShopItemAssetType();

	UTexture2D* GetItemIcon() const;

	FORCEINLINE FText GetItemName() const { return ItemName; }
	FORCEINLINE FText GetItemDescription() const { return ItemDescription; }
	FORCEINLINE float GetPrice() const { return ItemPrice; }
	FORCEINLINE float GetSellPrice() const { return ItemPrice / 2.f; }

	FORCEINLINE TSubclassOf<UGameplayEffect> GetEquippedEffect() const { return EquippedEffect; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetConsumeEffect() const { return ConsumeEffect; }
	FORCEINLINE TSubclassOf<UGameplayAbility> GetGrantedAbility() const { return GrantedAbility; }

	FORCEINLINE bool GetIsStackable() const { return bIsStackable; }
	FORCEINLINE bool GetIsConsumable() const { return bIsConsumable; }
	FORCEINLINE int GetMaxStackCount() const { return MaxStackCount; }
	FORCEINLINE TArray<TSoftObjectPtr<UShopItemAsset>> GetIngredientItems() const { return IngredientItems; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSoftObjectPtr<UTexture2D> ItemSoftIcon;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	float ItemPrice;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsConsumable;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayEffect> EquippedEffect;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayEffect> ConsumeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayAbility> GrantedAbility;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsStackable = false;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	int MaxStackCount = 5;

	//可以被哪些Item合成.
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TArray<TSoftObjectPtr<UShopItemAsset>> IngredientItems;
};
