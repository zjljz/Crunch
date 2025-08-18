// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityToolTip.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UAbilityToolTip::SetAbilityInfo(const FName& AbilityName, UTexture2D* AbilityIcon,const FText& AbilityDescription, float AbilityCooldown, float AbilityCost) const
{
	Text_AbilityName->SetText(FText::FromName(AbilityName));
	Img_AbilityIcon->SetBrushFromTexture(AbilityIcon);
	Text_AbilityDescription->SetText(AbilityDescription);

	FNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = 0;
	
	Text_AbilityCooldown->SetText(FText::AsNumber(AbilityCooldown, &Options));
	Text_AbilityCost->SetText(FText::AsNumber(AbilityCost, &Options));
}
