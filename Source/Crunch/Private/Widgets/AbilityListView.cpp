// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityListView.h"

#include "Abilities/GameplayAbility.h"
#include "Widgets/AbilityGauge.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	OnEntryWidgetGenerated().AddUObject(this, &ThisClass::AbilityGaugeGenerated);
	
	for (const TPair<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>> Pair : Abilities)
	{
		AddItem(Pair.Value.GetDefaultObject());
	}
}

const FAbilityWidgetDataRow* UAbilityListView::GetAbilityWidgetDataForAbility(const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	if (!DT_AbilityWidgetData)
	{
		return nullptr;
	}

	for (auto& Row : DT_AbilityWidgetData->GetRowMap())
	{
		const FAbilityWidgetDataRow* AbilityWidgetData = DT_AbilityWidgetData->FindRow<FAbilityWidgetDataRow>(Row.Key, TEXT(""));
		if (AbilityWidgetData && AbilityWidgetData->AbilityClass == AbilityClass)
		{
			return AbilityWidgetData;
		}
	}

	return nullptr;
}

void UAbilityListView::AbilityGaugeGenerated(UUserWidget& Widget)
{
	if (UAbilityGauge* AbilityGauge = Cast<UAbilityGauge>(&Widget))
	{
		if (const FAbilityWidgetDataRow* AbilityData = GetAbilityWidgetDataForAbility(AbilityGauge->GetListItem<UGameplayAbility>()->GetClass()))
		{
			AbilityGauge->ConfigureWithWidgetData(AbilityData);
		}
	}
}
