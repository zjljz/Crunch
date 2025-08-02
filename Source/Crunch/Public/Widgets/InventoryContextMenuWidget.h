// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryContextMenuWidget.generated.h"

class FOnButtonClickedEvent;
class UButton;
/**
 * 
 */
UCLASS()
class CRUNCH_API UInventoryContextMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnButtonClickedEvent& GetSellButtonClickedEvent() const;
	FOnButtonClickedEvent& GetUseButtonClickedEvent() const;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Use;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Sell;
};
