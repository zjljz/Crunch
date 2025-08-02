// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryItem.h"
#include "InventoryComponent.generated.h"


class UShopItemAsset;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddDelegate, const UInventoryItem* /* NewItem */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemoveDelegate, const FInventoryItemHandle& /* RemoveItemHandle */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemStackCountChangedDelegate, const FInventoryItemHandle&, int /* NewCount */);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRUNCH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	float GetGold() const;

	FORCEINLINE int GetCapacity() const { return Capacity; }

	//购买某个物品.
	void TryBuySth(const UShopItemAsset* ItemToBuy);

	//当某个Slot的物品发生变动时调用该函数.
	void OnInventorySlotChanged(const FInventoryItemHandle& Handle, int NewSlotIndex);

	//通过Handle来找到对应的Item实例.
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;

	//尝试使用某个Item.
	void TryActivateItem(const FInventoryItemHandle& ItemHandle);

	void SellItem(const FInventoryItemHandle& Handle);

	bool IsAllSlotFull() const;

	bool IsFullForItem(const UShopItemAsset* Item);

	UInventoryItem* GetAvailableStackForItem(const UShopItemAsset* Item) const;

	/**
	 * @param Item 要合成的Item
	 * @param OutIngredients 如果这个Item当前可合成 那么返回Inventory中 这个Item的Ingredients.
	 * @param IngredientToIgnore 需要忽略查找的Ingredient,
	 * @return 这个Item当前是否可以合成.
	 */
	bool FoundIngredientForItem(const UShopItemAsset* Item, TArray<UInventoryItem*>& OutIngredients,
	                            const TArray<const UShopItemAsset*>& IngredientToIgnore = TArray<const UShopItemAsset*>{});

	UInventoryItem* TryGetItemByShopItemAsset(const UShopItemAsset* ItemAsset) const;

public:
	FOnItemAddDelegate OnItemAdd;

	FOnItemRemoveDelegate OnItemRemove;

	FOnItemStackCountChangedDelegate OnItemStackCountChanged;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;

	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int Capacity = 6;


	/*********************************************/
	/*					Server					**/
	/*********************************************/
private:
	//Server购买物品,如果物品栏已满,会尝试是否可以组成更高级的Item.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_BuySth(const UShopItemAsset* ItemToBuy);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ActivateItem(FInventoryItemHandle ItemHandle);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SellItem(FInventoryItemHandle ItemHandle);

	//Server Add Item.
	void GrantItem(const UShopItemAsset* NewItem);

	//Server消耗Item 
	void ConsumeItem(UInventoryItem* Item);

	//Server移除Item
	void RemoveItem(UInventoryItem* ItemToRemove);

	//Server检查NewItem是否可以合成别的Item,如果可以 那么直接合成 减少一次购买.
	bool TryItemCombination(const UShopItemAsset* NewItemAsset);

	/*************************************************/
	/*                   Client                      */
	/*************************************************/

	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UShopItemAsset* NewItem);

	UFUNCTION(Client, Reliable)
	void Client_ItemRemoved(FInventoryItemHandle RemoveItemHandle);

	UFUNCTION(Client, Reliable)
	void Client_ItemStackCountChanged(FInventoryItemHandle Handle, int NewCount);
};
