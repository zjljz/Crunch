// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"

#include "AbilitySystemComponent.h"
#include "ShopItemAsset.h"
#include "AbilitySystem/CrunchAbilitySystemStatics.h"

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

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UShopItemAsset* NewShopItem, UAbilitySystemComponent* InASC)
{
	ItemHandle = NewHandle;
	ShopItem = NewShopItem;

	OwnerASC = InASC;
	ApplyGASModifications();
}

void UInventoryItem::ApplyGASModifications()
{
	if (!GetShopItem() || !OwnerASC) return;

	if (!OwnerASC->GetOwner() || !OwnerASC->GetOwner()->HasAuthority()) return;

	if (TSubclassOf<UGameplayEffect> EquipEffectClass = GetShopItem()->GetEquippedEffect())
	{
		EquippedGEHandle = OwnerASC->BP_ApplyGameplayEffectToSelf(EquipEffectClass, 1.f, OwnerASC->MakeEffectContext());
	}

	if (TSubclassOf<UGameplayAbility> GAClass = GetShopItem()->GetGrantedAbility())
	{
		//@todo: 如果这里添加了相同的Item, 会重复授予相同的Ability.
		GrantedAbilitySpecHandle = OwnerASC->GiveAbility(FGameplayAbilitySpec(GAClass));
	}
}

void UInventoryItem::ApplyConsumeEffect()
{
	if (!ShopItem) return;

	if (TSubclassOf<UGameplayEffect> ConsumeEffectClass = GetShopItem()->GetConsumeEffect())
	{
		ConsumeGEHandle = OwnerASC->BP_ApplyGameplayEffectToSelf(ConsumeEffectClass, 1.f, OwnerASC->MakeEffectContext());
	}
}

bool UInventoryItem::TryActivateGrantedAbility() const
{
	if (!GrantedAbilitySpecHandle.IsValid()) return false;

	if (OwnerASC && OwnerASC->TryActivateAbility(GrantedAbilitySpecHandle)) return true;

	return false;
}

void UInventoryItem::RemoveGASModification()
{
	if (!OwnerASC) return;

	if (EquippedGEHandle.IsValid())
	{
		OwnerASC->RemoveActiveGameplayEffect(EquippedGEHandle);
	}

	if (GrantedAbilitySpecHandle.IsValid())
	{
		OwnerASC->SetRemoveAbilityOnEnd(GrantedAbilitySpecHandle);
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

bool UInventoryItem::IsGrantedAnyAbility() const
{
	return GetShopItem() && GetShopItem()->GetGrantedAbility() && GrantedAbilitySpecHandle.IsValid();
}

float UInventoryItem::GetAbilityCooldownTimeRemaining() const
{
	if (!IsGrantedAnyAbility()) return 0.f;
	
	return UCrunchAbilitySystemStatics::GetCooldownRemainingForAbility(GetShopItem()->GetGrantedAbilityCDO(), *OwnerASC);
}

float UInventoryItem::GetAbilityCooldownDuration() const
{
	if (!IsGrantedAnyAbility()) return 0.f;
	
	return UCrunchAbilitySystemStatics::GetCooldownDurationForAbility(GetShopItem()->GetGrantedAbilityCDO(), *OwnerASC, 1);
}

float UInventoryItem::GetAbilityManaCost() const
{
	if (!IsGrantedAnyAbility()) return 0.f;
	return UCrunchAbilitySystemStatics::GetManaCostForAbility(GetShopItem()->GetGrantedAbilityCDO(), *OwnerASC, 1);
}
