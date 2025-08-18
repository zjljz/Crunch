// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityToolTip.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class CRUNCH_API UAbilityToolTip : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetAbilityInfo(const FName& AbilityName, UTexture2D* AbilityIcon,const FText& AbilityDescription, float AbilityCooldown, float AbilityCost) const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AbilityName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_AbilityIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AbilityDescription;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AbilityCooldown;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AbilityCost;
};
