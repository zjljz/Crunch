// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"

#include "AbilitySystemComponent.h"
#include "ShopItemAsset.h"

FInventoryItemHandle::FInventoryItemHandle() : HandleId(GetInvalidId())
{
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
	return FInventoryItemHandle(GenerateNextId());
}

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	static FInventoryItemHandle InvalidHandle = FInventoryItemHandle();
	return InvalidHandle;
}

FInventoryItemHandle::FInventoryItemHandle(uint32 InId) : HandleId(InId)
{
}

uint32 FInventoryItemHandle::GenerateNextId()
{
	static uint32 StaticId = 1;
	return StaticId++;
}

uint32 FInventoryItemHandle::GetInvalidId()
{
	return 0;
}

bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs)
{
	return Lhs.GetHandleId() == Rhs.GetHandleId();
}

uint32 GetTypeHash(const FInventoryItemHandle& Key)
{
	return Key.GetHandleId();
}


/*******************************************      UInventoryItem  ********************************/


UInventoryItem::UInventoryItem() : StackCount(1), SlotIndex(-1)
{
}

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UShopItemAsset* NewShopItem)
{
	ItemHandle = NewHandle;
	ShopItem = NewShopItem;
}

void UInventoryItem::ApplyGASModifications(UAbilitySystemComponent* ASC)
{
	if (!GetShopItem() || !ASC) return;

	if (!ASC->GetOwner() || !ASC->GetOwner()->HasAuthority()) return;

	if (TSubclassOf<UGameplayEffect> EquipEffectClass = GetShopItem()->GetEquippedEffect())
	{
		ActiveGEHandle = ASC->BP_ApplyGameplayEffectToSelf(EquipEffectClass, 1.f, ASC->MakeEffectContext());
	}

	if (TSubclassOf<UGameplayAbility> GAClass = GetShopItem()->GetGrantedAbility())
	{
		FGameplayAbilitySpec* FoundAbilitySpec = ASC->FindAbilitySpecFromClass(GAClass);
		if (FoundAbilitySpec)
		{
			GrantedAbilitySpecHandle = FoundAbilitySpec->Handle;
		}
		else
		{
			GrantedAbilitySpecHandle = ASC->GiveAbility(FGameplayAbilitySpec(GAClass));
		}
	}
}

bool UInventoryItem::IsValid() const
{
	return ShopItem != nullptr;
}

void UInventoryItem::SetSlot(int NewSlotIndex)
{
	SlotIndex = NewSlotIndex;
}

bool UInventoryItem::IsStackFull() const
{
	return StackCount >= ShopItem->GetMaxStackCount();
}

bool UInventoryItem::IsSameAsset(const UShopItemAsset* InItem) const
{
	return ShopItem == InItem;
}

bool UInventoryItem::AddStackCount()
{
	if (IsStackFull()) return false;

	++StackCount;

	return true;
}

bool UInventoryItem::ReduceStackCount()
{
	--StackCount;
	if (StackCount <= 0) return false;

	return true;
}

bool UInventoryItem::SetStackCount(int NewStackCount)
{
	if (NewStackCount > 0 && NewStackCount <= GetShopItem()->GetMaxStackCount())
	{
		StackCount = NewStackCount;
		return true;
	}

	return false;
}
