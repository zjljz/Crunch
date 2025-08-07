// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "InventoryItem.generated.h"

struct FOnAttributeChangeData;
struct FGameplayAbilitySpec;
class UAbilitySystemComponent;
class UShopItemAsset;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityCanCastUpdateDelegate, bool /* bCanCast */);

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
	void InitItem(const FInventoryItemHandle& NewHandle, const UShopItemAsset* NewShopItem, UAbilitySystemComponent* InASC);

	//调用Item自带的GE或要授予的Ability.
	void ApplyGASModifications();
	void ApplyConsumeEffect();

	//尝试激活授予的Ability.
	bool TryActivateGrantedAbility() const;

	//移除EquippedGE 以及 GrantedGA.
	void RemoveGASModification();

	void OnManaAttributeUpdate(const FOnAttributeChangeData& Data);

	bool IsValid() const;

	//设置Item在装备栏/背包的位置.
	void SetSlot(int NewSlotIndex);

	bool IsStackFull() const;

	bool IsSameAsset(const UShopItemAsset* InItem) const;

	bool AddStackCount();

	//如果减少后 不为空 那么返回true.
	bool ReduceStackCount();

	bool SetStackCount(int NewStackCount);

	bool IsGrantedAnyAbility() const;

	float GetAbilityCooldownTimeRemaining() const;
	float GetAbilityCooldownDuration() const;
	float GetAbilityManaCost() const;
	bool CanCastAbility() const;
	FGameplayAbilitySpecHandle GetGrantedAbilitySpecHandle() const { return GrantedAbilitySpecHandle; }
	void SetGrantedAbilitySpecHandle(FGameplayAbilitySpecHandle SpecHandle) { GrantedAbilitySpecHandle = SpecHandle; };

	FORCEINLINE const UShopItemAsset* GetShopItem() const { return ShopItem; }
	FORCEINLINE FInventoryItemHandle GetHandle() const { return ItemHandle; }
	FORCEINLINE int GetStackCount() const { return StackCount; }
	FORCEINLINE int GetSlotIndex() const { return SlotIndex; }

public:
	FOnAbilityCanCastUpdateDelegate OnAbilityCanCastUpdate;

private:
	//这个Item对应的资产.
	UPROPERTY()
	TObjectPtr<const UShopItemAsset> ShopItem;

	UPROPERTY()
	FInventoryItemHandle ItemHandle;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;

	int StackCount;
	int SlotIndex;

	//装备对应的GEHandle.
	FActiveGameplayEffectHandle EquippedGEHandle;

	//消耗品对应的GEHandle, 一般消耗品都是Duration/InstanceGE 可能不需要这个Handle去取消.
	FActiveGameplayEffectHandle ConsumeGEHandle;

	//装备自带的Ability授予后的Handle.
	FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;
};
