// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ShopItemAsset.h"
#include "AbilitySystem/CrunchHeroAttributeSet.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}

float UInventoryComponent::GetGold() const
{
	float Ret = 0.f;
	if (OwnerASC)
	{
		bool bFound = false;
		Ret = OwnerASC->GetGameplayAttributeValue(UCrunchHeroAttributeSet::GetGoldAttribute(), bFound);
	}

	return Ret;
}


void UInventoryComponent::TryBuySth(const UShopItemAsset* ItemToBuy)
{
	if (!OwnerASC) return;

	Server_BuySth(ItemToBuy);
}

void UInventoryComponent::OnInventorySlotChanged(const FInventoryItemHandle& Handle, int NewSlotIndex)
{
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		FoundItem->SetSlot(NewSlotIndex);
	}
}

UInventoryItem* UInventoryComponent::GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const
{
	if (UInventoryItem* const* FoundItem = InventoryMap.Find(Handle))
	{
		return *FoundItem;
	}

	return nullptr;
}

bool UInventoryComponent::IsAllSlotFull() const
{
	return InventoryMap.Num() >= GetCapacity();
}

bool UInventoryComponent::IsFullForItem(const UShopItemAsset* Item)
{
	if (!Item) return false;

	if (IsAllSlotFull())
	{
		return GetAvailableStackForItem(Item) == nullptr;
	}

	return false;
}

UInventoryItem* UInventoryComponent::GetAvailableStackForItem(const UShopItemAsset* Item) const
{
	if (!Item->GetIsStackable()) return nullptr;

	for (const TPair<FInventoryItemHandle, UInventoryItem*>& Pair : InventoryMap)
	{
		if (Pair.Value && Pair.Value->IsSameAsset(Item) && !Pair.Value->IsStackFull())
		{
			return Pair.Value;
		}
	}

	return nullptr;
}

void UInventoryComponent::Server_BuySth_Implementation(const UShopItemAsset* ItemToBuy)
{
	//@todo: 这里不管是购买失败 还是购买成功 应该都需要一个Log 或者更多.
	if (!ItemToBuy) return;

	if (GetGold() < ItemToBuy->GetPrice()) return;

	if (IsFullForItem(ItemToBuy)) return;

	OwnerASC->ApplyModToAttribute(UCrunchHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToBuy->GetPrice());

	GrantItem(ItemToBuy);
}

bool UInventoryComponent::Server_BuySth_Validate(const UShopItemAsset* ItemToBuy)
{
	return true;
}

void UInventoryComponent::GrantItem(const UShopItemAsset* NewItem)
{
	if (!GetOwner()->HasAuthority()) return;

	if (UInventoryItem* StackItem = GetAvailableStackForItem(NewItem))
	{
		StackItem->AddStackCount();
		OnItemStackCountChanged.Broadcast(StackItem->GetHandle(), StackItem->GetStackCount());
		Client_ItemStackCountChanged(StackItem->GetHandle(), StackItem->GetStackCount());
	}
	else
	{
		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();

		InventoryItem->InitItem(NewHandle, NewItem);
		InventoryMap.Add(NewHandle, InventoryItem);
		OnItemAdd.Broadcast(InventoryItem);

		//@todo: 这里可以添加一个Log.
		UE_LOG(LogTemp, Warning, TEXT("Server Add ShopItem : %s, with Id :%d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());

		Client_ItemAdded(NewHandle, NewItem);
		InventoryItem->ApplyGASModifications(OwnerASC);
	}
}

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UShopItemAsset* NewItem)
{
	if (GetOwner()->HasAuthority()) return;

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(AssignedHandle, NewItem);

	InventoryMap.Add(AssignedHandle, InventoryItem);
	OnItemAdd.Broadcast(InventoryItem);

	UE_LOG(LogTemp, Warning, TEXT("Client Add ShopItem : %s, with Id :%d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), AssignedHandle.GetHandleId());
}

void UInventoryComponent::Client_ItemStackCountChanged_Implementation(FInventoryItemHandle Handle, int NewCount)
{
	if (GetOwner()->HasAuthority()) return;

	UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle);
	if (FoundItem)
	{
		FoundItem->SetStackCount(NewCount);
		OnItemStackCountChanged.Broadcast(Handle, NewCount);
	}
}
