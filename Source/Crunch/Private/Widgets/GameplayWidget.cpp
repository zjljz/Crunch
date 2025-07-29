// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"

#include "AbilitySystemGlobals.h"
#include "Crunch/Public/AbilitySystem/CrunchAbilitySystemComponent.h"
#include "Widgets/AbilityListView.h"
#include "Widgets/InventoryShopWidget.h"
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

void UGameplayWidget::ToggleShopVisible()
{
	if (ShopWidget->GetVisibility() == ESlateVisibility::HitTestInvisible)
	{
		ShopWidget->SetVisibility(ESlateVisibility::Visible);
		PlayShopPopAnimation();
		SetOwningPawnInputEnabled(false);
		SetShowMouseCursor(true);
		SetInputModeGameAndUI();
		ShopWidget->SetFocus();
	}
	else
	{
		ShopWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayShopPopAnimation(false);
		SetOwningPawnInputEnabled(true);
		SetShowMouseCursor(false);
		SetInputModeGameOnly();
	}
}

void UGameplayWidget::PlayShopPopAnimation(bool bPlayForward)
{
	if (bPlayForward)
	{
		PlayAnimationForward(ShopPopAnim);
	}
	else
	{
		PlayAnimationReverse(ShopPopAnim);
	}
}

void UGameplayWidget::SetOwningPawnInputEnabled(bool bEnabled)
{
	if (bEnabled)
	{
		GetOwningPlayerPawn()->EnableInput(GetOwningPlayer());
	}
	else
	{
		GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
	}
}

void UGameplayWidget::SetShowMouseCursor(bool bShowMouseCursor)
{
	GetOwningPlayer()->SetShowMouseCursor(bShowMouseCursor);
}

void UGameplayWidget::SetInputModeGameAndUI()
{
	FInputModeGameAndUI GameAndUIInputMode;
	GameAndUIInputMode.SetHideCursorDuringCapture(false);

	GetOwningPlayer()->SetInputMode(GameAndUIInputMode);
}

void UGameplayWidget::SetInputModeGameOnly()
{
	FInputModeGameOnly GameOnlyInputMode;

	GetOwningPlayer()->SetInputMode(GameOnlyInputMode);
}
