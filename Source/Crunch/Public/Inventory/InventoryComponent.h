// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryItem.h"
#include "InventoryComponent.generated.h"


class UShopItemAsset;
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddDelegate, const UInventoryItem* /* NewItem */);
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

	bool IsAllSlotFull() const;

	bool IsFullForItem(const UShopItemAsset* Item);
	
	UInventoryItem* GetAvailableStackForItem(const UShopItemAsset* Item) const;

public:
	FOnItemAddDelegate OnItemAdd;

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
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_BuySth(const UShopItemAsset* ItemToBuy);

	void GrantItem(const UShopItemAsset* NewItem);


	/*************************************************/
	/*                   Client                      */
	/*************************************************/

	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UShopItemAsset* NewItem);

	UFUNCTION(Client, Reliable)
	void Client_ItemStackCountChanged(FInventoryItemHandle Handle, int NewCount);
};
