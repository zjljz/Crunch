// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "StatsGauge.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class CRUNCH_API UStatsGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	void SetValueText(float NewValue);

	void OnAttributeValueChanged(const FOnAttributeChangeData& Data);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AttributeText;
	
	FNumberFormattingOptions NumFormattingOp;
	
	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute Attribute;
	
	UPROPERTY(EditAnywhere, Category = "Visual")
	TObjectPtr<UTexture2D> IconTexture;
};
