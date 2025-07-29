// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


class UShopItemAsset;
class UAbilitySystemComponent;

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
	
	//购买某个物品.
	void TryBuySth(const UShopItemAsset* ItemToBuy);

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;

	/*************************************************/
	/*************** Server Function *****************/
	/*************************************************/
private:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_BuySth(const UShopItemAsset* ItemToBuy);
};
