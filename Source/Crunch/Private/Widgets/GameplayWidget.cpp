// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"

#include "AbilitySystemGlobals.h"
#include "Crunch/Public/AbilitySystem/CrunchAbilitySystemComponent.h"
#include "Widgets/AbilityListView.h"
#include "Widgets/ValueGauge.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		HealthGauge->SetAndBoundToGameplayAttribute(OwnerASC, UCrunchAttributeSet::GetHealthAttribute(), UCrunchAttributeSet::GetMaxHealthAttribute());
		ManaGauge->SetAndBoundToGameplayAttribute(OwnerASC, UCrunchAttributeSet::GetManaAttribute(), UCrunchAttributeSet::GetMaxManaAttribute());
	}

	if (UCrunchAbilitySystemComponent* CrunchASC = Cast<UCrunchAbilitySystemComponent>(OwnerASC))
	{
		ConfigureAbilities(CrunchASC->GetAbilities());
	}
}

void UGameplayWidget::ConfigureAbilities(const TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
	AbilityListView->ConfigureAbilities(Abilities);	
}

