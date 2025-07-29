// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTip.generated.h"

class UShopItemAsset;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API UItemToolTip : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetItem(const UShopItemAsset* Item);
	void SetPrice(float NewPrice);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_ItemIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ItemName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ItemDescription;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ItemPrice;
};
