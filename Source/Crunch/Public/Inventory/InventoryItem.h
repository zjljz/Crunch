// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "InventoryItem.generated.h"

struct FGameplayAbilitySpec;
class UAbilitySystemComponent;
class UShopItemAsset;

/**
 *  用来在网络中传输的 Handle, 避免直接传输UInventoryItem
 */
USTRUCT()
struct FInventoryItemHandle
{
	GENERATED_BODY()

public:
	FInventoryItemHandle();

	static FInventoryItemHandle CreateHandle();
	static FInventoryItemHandle InvalidHandle();

	FORCEINLINE bool IsValid() const { return HandleId != GetInvalidId(); }

	FORCEINLINE uint32 GetHandleId() const { return HandleId; }

private:
	explicit FInventoryItemHandle(uint32 InId);

	static uint32 GenerateNextId();
	static uint32 GetInvalidId();

private:
	UPROPERTY()
	uint32 HandleId;
};

bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs);

uint32 GetTypeHash(const FInventoryItemHandle& Key);

/**
 * Item的实例
 */
UCLASS()
class CRUNCH_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItem();

	//一般是在Server创建Item后 传递一个Handle给Client初始化
	void InitItem(const FInventoryItemHandle& NewHandle, const UShopItemAsset* NewShopItem);

	//调用Item自带的GE或要授予的Ability.
	void ApplyGASModifications(UAbilitySystemComponent* ASC);
	
	bool IsValid() const;

	//设置Item在装备栏/背包的位置.
	void SetSlot(int NewSlotIndex);

	bool IsStackFull() const;

	bool IsSameAsset(const UShopItemAsset* InItem) const;

	bool AddStackCount();

	//如果减少后 不为空 那么返回true.
	bool ReduceStackCount();

	bool SetStackCount(int NewStackCount);
	
	FORCEINLINE const UShopItemAsset* GetShopItem() const { return ShopItem; }
	FORCEINLINE FInventoryItemHandle GetHandle() const { return ItemHandle; }
	FORCEINLINE int GetStackCount() const { return StackCount; }

private:
	//这个Item对应的资产.
	UPROPERTY()
	TObjectPtr<const UShopItemAsset> ShopItem;
	
	UPROPERTY()
	FInventoryItemHandle ItemHandle;

	int StackCount;
	int SlotIndex;

	//
	FActiveGameplayEffectHandle ActiveGEHandle;

	//
	FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;
};
