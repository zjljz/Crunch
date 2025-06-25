// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OverHeadStatsGauge.h"

#include "GAS/CrunchAttributeSet.h"
#include "Widgets/ValueGauge.h"

void UOverHeadStatsGauge::ConfigureWithASC(UAbilitySystemComponent* ASC)
{
	if (ASC)
	{
		HealthGauge->SetAndBoundToGameplayAttribute(ASC, UCrunchAttributeSet::GetHealthAttribute(), UCrunchAttributeSet::GetMaxHealthAttribute());
		ManaGauge->SetAndBoundToGameplayAttribute(ASC, UCrunchAttributeSet::GetManaAttribute(), UCrunchAttributeSet::GetMaxManaAttribute());
	}
}
