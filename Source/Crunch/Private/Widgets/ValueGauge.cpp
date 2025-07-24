// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ValueGauge.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	ProgressBar->SetFillColorAndOpacity(BarColor);

	ValueText->SetFont(ValueTextFont);
	ValueText->SetVisibility(bValueTextVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	ProgressBar->SetVisibility(bProgressBarVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UValueGauge::NativeConstruct()
{
	Super::NativeConstruct();
}

void UValueGauge::SetAndBoundToGameplayAttribute(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
	if (ASC)
	{
		bool bFound = false;
		float Value = ASC->GetGameplayAttributeValue(Attribute, bFound);
		float MaxValue = ASC->GetGameplayAttributeValue(MaxAttribute, bFound);
		if (bFound)
		{
			SetValue(Value, MaxValue);
		}

		//值得注意的是 这里的委托其实是在AttributeSet中 OnRep函数触发的.
		ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UValueGauge::OnValueChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UValueGauge::OnMaxValueChanged);
	}
}

void UValueGauge::SetValue(float NewVal, float NewMaxVal)
{
	CurVal = NewVal;
	MaxVal = NewMaxVal;
	
	if (NewMaxVal == 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("ValueGauge: %s, NewMaxVal is zero, cannot set value."), *GetNameSafe(this));
		return;
	}

	float NewPercent = NewVal / NewMaxVal;
	ProgressBar->SetPercent(NewPercent);

	FNumberFormattingOptions Options = FNumberFormattingOptions().SetMaximumFractionalDigits(0);

	ValueText->SetText(FText::Format(FTextFormat::FromString("{0}/{1}"),
	                                 FText::AsNumber(NewVal, &Options),
	                                 FText::AsNumber(NewMaxVal, &Options)));
}

