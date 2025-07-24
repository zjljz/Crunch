// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/CrunchAbilitySystemStatics.h"


void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();

	CooldownCountText->SetVisibility(ESlateVisibility::Collapsed);

	if (UAbilitySystemComponent* OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwningPlayerPawn()))
	{
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &ThisClass::OnAbilityCommitted);
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
