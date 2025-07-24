// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/StatsGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UStatsGauge::NativePreConstruct()
{
	Super::NativePreConstruct();

	Icon->SetBrushFromTexture(IconTexture);
}

void UStatsGauge::NativeConstruct()
{
	Super::NativeConstruct();

	NumFormattingOp.MaximumFractionalDigits = 0;
	
	if (!GetOwningPlayerPawn()) return;

	if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn()))
	{
		bool bFound = false;
		float NewValue = OwnerASC->GetGameplayAttributeValue(Attribute, bFound);
		SetValueText(NewValue);

		OwnerASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ThisClass::OnAttributeValueChanged);
	}
}

void UStatsGauge::SetValueText(float NewValue)
{
	AttributeText->SetText(FText::AsNumber(NewValue, &NumFormattingOp));
}

void UStatsGauge::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
	SetValueText(Data.NewValue);	
}
