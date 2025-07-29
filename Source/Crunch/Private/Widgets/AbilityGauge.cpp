// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/CrunchAttributeSet.h"
#include "AbilitySystem/CrunchHeroAttributeSet.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Crunch/Public/AbilitySystem/CrunchAbilitySystemStatics.h"


void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();

	CooldownCountText->SetVisibility(ESlateVisibility::Collapsed);

	OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::OnAbilityCommitted);
		OwnerASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &ThisClass::OnAbilitySpecUpdate);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UCrunchHeroAttributeSet::GetUpgradePointAttribute()).AddUObject(this, &ThisClass::OnUpgradePointUpdate);
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UCrunchAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaUpdate);
	}

	bool bFound = false;
	float UpdatePoint = OwnerASC->GetGameplayAttributeValue(UCrunchHeroAttributeSet::GetUpgradePointAttribute(), bFound);
	if (bFound)
	{
		FOnAttributeChangeData Data;
		Data.NewValue = UpdatePoint;
		Data.Attribute = UCrunchHeroAttributeSet::GetUpgradePointAttribute();
		OnUpgradePointUpdate(Data);
	}

	WholeNumberFormattingOptions.MaximumFractionalDigits = 0;
	TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 2;
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

	float CooldownDuration = UCrunchAbilitySystemStatics::GetCooldownDurationForAbility(AbilityCDO.Get());
	float Cost = UCrunchAbilitySystemStatics::GetCostForAbility(AbilityCDO.Get());

	CachedCooldownDuration = CooldownDuration;
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CostText->SetText(FText::AsNumber(Cost));

	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, 0);
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetDataRow* AbilityWidgetData)
{
	if (!AbilityWidgetData)
	{
		return;
	}

	if (Icon && AbilityWidgetData->Icon.ToSoftObjectPath().IsValid())
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, AbilityWidgetData->Icon.LoadSynchronous());
	}
}

void UAbilityGauge::OnAbilityCommitted(UGameplayAbility* Ability)
{
	if (Ability->GetClass()->GetDefaultObject() == AbilityCDO.Get())
	{
		float CooldownTimeRemaining = 0.f;
		float CooldownDuration = 0.f;

		Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(),
		                                             CooldownTimeRemaining, CooldownDuration);

		StartCooldown(CooldownTimeRemaining, CooldownDuration);
	}
}

void UAbilityGauge::StartCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	if (CooldownTimeRemaining <= 0.f || CooldownDuration <= 0.f)
	{
		return; // No cooldown to start
	}

	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CachedCooldownDuration = CooldownDuration;
	CachedCooldownRemainingTime = CooldownTimeRemaining;

	CooldownCountText->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ThisClass::CooldownFinished, CooldownTimeRemaining);
	GetWorld()->GetTimerManager().SetTimer(CooldownUpdateTimerHandle, this, &ThisClass::UpdateCooldown, CooldownUpdateInterval, true, 0.f);
}

void UAbilityGauge::CooldownFinished()
{
	CooldownCountText->SetVisibility(ESlateVisibility::Collapsed);

	CachedCooldownRemainingTime = 0.f;
	CachedCooldownDuration = 0.f;

	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.f);
}

void UAbilityGauge::UpdateCooldown()
{
	CachedCooldownRemainingTime -= CooldownUpdateInterval;

	FNumberFormattingOptions* Options = CachedCooldownRemainingTime > 1.f ? &WholeNumberFormattingOptions : &TwoDigitNumberFormattingOptions;

	CooldownCountText->SetText(FText::AsNumber(CachedCooldownRemainingTime, Options));

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamName, 1.f - CachedCooldownRemainingTime / CachedCooldownDuration);
}

const FGameplayAbilitySpec* UAbilityGauge::GetAbilitySpec()
{
	if (!CachedAbilitySpec)
	{
		if (AbilityCDO.Get() && OwnerASC)
		{
			CachedAbilitySpec = OwnerASC->FindAbilitySpecFromClass(AbilityCDO->GetClass());
		}
	}

	return CachedAbilitySpec;
}

void UAbilityGauge::OnAbilitySpecUpdate(const FGameplayAbilitySpec& NewSpec)
{
	if (NewSpec.Ability != AbilityCDO)
	{
		return;
	}

	bIsAbilityLearned = NewSpec.Level > 0;
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, NewSpec.Level);
	UpdateCanCast();

	float NewCooldownDuration = UCrunchAbilitySystemStatics::GetCooldownDurationForAbility(NewSpec.Ability, *OwnerASC, NewSpec.Level);
	float NewCost = UCrunchAbilitySystemStatics::GetManaCostForAbility(NewSpec.Ability, *OwnerASC, NewSpec.Level);

	CooldownDurationText->SetText(FText::AsNumber(NewCooldownDuration));
	CostText->SetText(FText::AsNumber(NewCost));
}

void UAbilityGauge::UpdateCanCast()
{
	bool bCanCast = bIsAbilityLearned;

	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec())
	{
		if (OwnerASC && !UCrunchAbilitySystemStatics::CheckAbilityCost(*AbilitySpec, *OwnerASC))
		{
			bCanCast = false;
		}
	}

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CanCastAbilityParamName, bCanCast ? 1 : 0);
}

void UAbilityGauge::OnUpgradePointUpdate(const FOnAttributeChangeData& Data)
{
	int Available = Data.NewValue > 0 ? 1 : 0;
	
	if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec())
	{
		if (UCrunchAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec))
		{
			Available = 0;
		}
	}

	Icon->GetDynamicMaterial()->SetScalarParameterValue(AvailableParamName, Available);
}

void UAbilityGauge::OnManaUpdate(const FOnAttributeChangeData& Data)
{
	UpdateCanCast();
}
