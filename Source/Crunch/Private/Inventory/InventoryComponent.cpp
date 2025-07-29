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

void UInventoryComponent::Server_BuySth_Implementation(const UShopItemAsset* ItemToBuy)
{
	//@todo: 这里不管是购买失败 还是购买成功 应该都需要一个Log 或者更多.
	if (!ItemToBuy) return;

	if (GetGold() < ItemToBuy->GetPrice()) return;

	OwnerASC->ApplyModToAttribute(UCrunchHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToBuy->GetPrice());
}

bool UInventoryComponent::Server_BuySth_Validate(const UShopItemAsset* ItemToBuy)
{
	return true;
}
