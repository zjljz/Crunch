// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelGauge.generated.h"

struct FOnAttributeChangeData;
class UAbilitySystemComponent;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API ULevelGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void UpdateGauge(const FOnAttributeChangeData& Data);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_LevelProgress;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Level;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName MaterialParamName_Percent;

	FNumberFormattingOptions NumFormatting;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;
};
