// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LevelGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CrunchHeroAttributeSet.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void ULevelGauge::NativeConstruct()
{
	Super::NativeConstruct();

	NumFormatting.SetMaximumFractionalDigits(0);

	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn);
		if (OwnerASC)
		{
			UpdateGauge(FOnAttributeChangeData());
			OwnerASC->GetGameplayAttributeValueChangeDelegate(UCrunchHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
			OwnerASC->GetGameplayAttributeValueChangeDelegate(UCrunchHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
			OwnerASC->GetGameplayAttributeValueChangeDelegate(UCrunchHeroAttributeSet::GetPreLevelExperienceAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
			OwnerASC->GetGameplayAttributeValueChangeDelegate(UCrunchHeroAttributeSet::GetLevelAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
		}
	}
}

void ULevelGauge::UpdateGauge(const FOnAttributeChangeData& Data)
{
	bool bFound = false;
	float CurExp = OwnerASC->GetGameplayAttributeValue(UCrunchHeroAttributeSet::GetExperienceAttribute(), bFound);
	if (!bFound) return;

	float PreExp = OwnerASC->GetGameplayAttributeValue(UCrunchHeroAttributeSet::GetPreLevelExperienceAttribute(), bFound);
	if (!bFound) return;

	float NextExp = OwnerASC->GetGameplayAttributeValue(UCrunchHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);
	if (!bFound) return;

	float CurLevel = OwnerASC->GetGameplayAttributeValue(UCrunchHeroAttributeSet::GetLevelAttribute(), bFound);
	if (!bFound) return;
	
	float Percent = (CurExp - PreExp) / (NextExp - PreExp);

	if (NextExp == 0)
	{
		Percent = 1;
	}
		
	if (Img_LevelProgress)
	{
		Img_LevelProgress->GetDynamicMaterial()->SetScalarParameterValue(MaterialParamName_Percent, Percent);
	}

	if (Text_Level)
	{
		Text_Level->SetText(FText::AsNumber(CurLevel, &NumFormatting));
	}
}
