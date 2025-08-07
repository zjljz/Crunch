// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ShopItemAsset.h"
#include "AbilitySystem/CrunchHeroAttributeSet.h"
#include "Framework/CrunchAssetManager.h"

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

void UInventoryComponent::TryActivateItem(const FInventoryItemHandle& ItemHandle)
{
	UInventoryItem* Item = GetInventoryItemByHandle(ItemHandle);
	if (!Item) return;

	Server_ActivateItem(ItemHandle);
}

void UInventoryComponent::SellItem(const FInventoryItemHandle& Handle)
{
	Server_SellItem(Handle);
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

bool UInventoryComponent::FoundIngredientForItem(const UShopItemAsset* Item, TArray<UInventoryItem*>& OutIngredients, const TArray<const UShopItemAsset*>& IngredientToIgnore)
{
	const FItemCollection* ItemIngredients = UCrunchAssetManager::Get().GetIngredientForItem(Item);
	if (!ItemIngredients) return false;

	bool bAllFound = true;
	for (const UShopItemAsset* IngredientAsset : ItemIngredients->GetItems())
	{
		if (IngredientToIgnore.Contains(IngredientAsset)) continue;

		UInventoryItem* IngredientItem = TryGetItemByShopItemAsset(IngredientAsset);
		if (!IngredientItem)
		{
			bAllFound = false;
			break;;
		}
		OutIngredients.Add(IngredientItem);
	}

	return bAllFound;
}

UInventoryItem* UInventoryComponent::TryGetItemByShopItemAsset(const UShopItemAsset* ItemAsset) const
{
	if (ItemAsset)
	{
		for (auto Pair : InventoryMap)
		{
			if (Pair.Value && Pair.Value->GetShopItem() == ItemAsset)
			{
				return Pair.Value;
			}
		}
	}

	return nullptr;
}

void UInventoryComponent::TryActivateItemInSlot(int SlotIndex)
{
	for (TPair<FInventoryItemHandle, UInventoryItem*> Pair : InventoryMap)
	{
		if (Pair.Value->GetSlotIndex() == SlotIndex)
		{
			Server_ActivateItem(Pair.Key);
			return;
		}
	}
}

void UInventoryComponent::Server_BuySth_Implementation(const UShopItemAsset* ItemToBuy)
{
	//@todo: 这里不管是购买失败 还是购买成功 应该都需要一个Log 或者更多.
	if (!ItemToBuy) return;

	if (GetGold() < ItemToBuy->GetPrice()) return;

	if (!IsFullForItem(ItemToBuy))
	{
		OwnerASC->ApplyModToAttribute(UCrunchHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToBuy->GetPrice());
		GrantItem(ItemToBuy);
		return;
	}

	if (TryItemCombination(ItemToBuy))
	{
		OwnerASC->ApplyModToAttribute(UCrunchHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToBuy->GetPrice());
	}
}

bool UInventoryComponent::Server_BuySth_Validate(const UShopItemAsset* ItemToBuy)
{
	return true;
}

void UInventoryComponent::Server_ActivateItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* Item = GetInventoryItemByHandle(ItemHandle);
	if (!Item) return;

	Item->TryActivateGrantedAbility();
	const UShopItemAsset* ItemAsset = Item->GetShopItem();

	if (ItemAsset->GetIsConsumable())
	{
		ConsumeItem(Item);
	}
}

bool UInventoryComponent::Server_ActivateItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

void UInventoryComponent::Server_SellItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* Item = GetInventoryItemByHandle(ItemHandle);

	if (!Item || !Item->IsValid()) return;

	float SellPrice = Item->GetShopItem()->GetSellPrice();

	OwnerASC->ApplyModToAttribute(UCrunchHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, SellPrice * Item->GetStackCount());
	RemoveItem(Item);
}

bool UInventoryComponent::Server_SellItem_Validate(FInventoryItemHandle ItemHandle)
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
		if (TryItemCombination(NewItem))
		{
			return;
		}

		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();

		InventoryItem->InitItem(NewHandle, NewItem, OwnerASC);
		InventoryMap.Add(NewHandle, InventoryItem);
		OnItemAdd.Broadcast(InventoryItem);

		//@todo: 这里可以添加一个Log.
		UE_LOG(LogTemp, Warning, TEXT("Server Add ShopItem : %s, with Id :%d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());

		FGameplayAbilitySpecHandle GrantedAbilitySpecHandle = InventoryItem->GetGrantedAbilitySpecHandle();
		Client_ItemAdded(NewHandle, NewItem, GrantedAbilitySpecHandle);
	}
}

void UInventoryComponent::ConsumeItem(UInventoryItem* Item)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (!Item) return;

	Item->ApplyConsumeEffect();

	if (!Item->ReduceStackCount())
	{
		RemoveItem(Item);
	}
	else
	{
		OnItemStackCountChanged.Broadcast(Item->GetHandle(), Item->GetStackCount());
		Client_ItemStackCountChanged(Item->GetHandle(), Item->GetStackCount());
	}
}

void UInventoryComponent::RemoveItem(UInventoryItem* ItemToRemove)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	ItemToRemove->RemoveGASModification();

	OnItemRemove.Broadcast(ItemToRemove->GetHandle());

	InventoryMap.Remove(ItemToRemove->GetHandle());

	Client_ItemRemoved(ItemToRemove->GetHandle());
}

bool UInventoryComponent::TryItemCombination(const UShopItemAsset* NewItemAsset)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return false;

	const FItemCollection* CombinationItems = UCrunchAssetManager::Get().GetCombinationForItem(NewItemAsset);
	if (!CombinationItems) return false;

	for (const UShopItemAsset* ItemAsset : CombinationItems->GetItems())
	{
		TArray<UInventoryItem*> Ingredients;
		if (!FoundIngredientForItem(ItemAsset, Ingredients, {NewItemAsset}))
		{
			continue;
		}

		for (UInventoryItem* Ingredient : Ingredients)
		{
			RemoveItem(Ingredient);
		}

		GrantItem(ItemAsset);
		return true;
	}

	return false;
}

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UShopItemAsset* NewItem, FGameplayAbilitySpecHandle SpecHandle)
{
	if (GetOwner()->HasAuthority()) return;

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(AssignedHandle, NewItem, OwnerASC);
	InventoryItem->SetGrantedAbilitySpecHandle(SpecHandle);

	InventoryMap.Add(AssignedHandle, InventoryItem);
	OnItemAdd.Broadcast(InventoryItem);

	UE_LOG(LogTemp, Warning, TEXT("Client Add ShopItem : %s, with Id :%d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), AssignedHandle.GetHandleId());
}

void UInventoryComponent::Client_ItemRemoved_Implementation(FInventoryItemHandle RemoveItemHandle)
{
	if (GetOwner()->HasAuthority()) return;

	UInventoryItem* Item = GetInventoryItemByHandle(RemoveItemHandle);
	if (!Item) return;

	Item->RemoveGASModification();

	OnItemRemove.Broadcast(RemoveItemHandle);
	InventoryMap.Remove(RemoveItemHandle);
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
