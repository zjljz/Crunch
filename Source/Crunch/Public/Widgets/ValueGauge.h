// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/CrunchAttributeSet.h"
#include "ValueGauge.generated.h"

class UAbilitySystemComponent;
struct FGameplayAttribute;
class UTextBlock;
class UProgressBar;
struct FOnAttributeChangeData;
/**
 * 
 */
UCLASS()
class CRUNCH_API UValueGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	void SetAndBoundToGameplayAttribute(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute);

	void SetValue(float NewVal, float NewMaxVal);

	void OnValueChanged(const FOnAttributeChangeData& ChangeData) { SetValue(ChangeData.NewValue, MaxVal); }
	void OnMaxValueChanged(const FOnAttributeChangeData& ChangeData){  SetValue(CurVal, ChangeData.NewValue); }
private:
	UPROPERTY(EditAnywhere, Category = "Visual")
	FLinearColor BarColor;

	UPROPERTY(EditAnywhere, Category = "Visual")
	FSlateFontInfo ValueTextFont;

	UPROPERTY(EditAnywhere, Category = "Visual")
	bool bValueTextVisible = true;

	UPROPERTY(EditAnywhere, Category = "Visual")
	bool bProgressBarVisible = true;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ValueText;

	
	
	float CurVal = 0.f;
	float MaxVal = 0.f;
};
