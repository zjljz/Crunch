// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"

class UCrosshairWidget;
class UCanvasPanel;
class USkeletalMeshRenderWidget;
class UInventoryWidget;
class UInventoryShopWidget;
class UStatsGauge;
class UAbilityListView;
class UGameplayAbility;
enum class ECrunchAbilityInputID : uint8;
class UAbilitySystemComponent;
class UValueGauge;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ConfigureAbilities(const TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities) const;

	void ToggleShopVisible();

private:
	void PlayShopPopAnimation(bool bPlayForward = true);

	void SetOwningPawnInputEnabled(bool bEnabled);

	void SetShowMouseCursor(bool bShowMouseCursor);

	void SetInputModeGameAndUI();

	void SetInputModeGameOnly();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityListView> AbilityListView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> AttackDamageGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> ArmorGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> StrengthGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> IntelligenceGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatsGauge> MoveSpeedGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryShopWidget> ShopWidget;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShopPopAnim;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkeletalMeshRenderWidget> HeadRenderActorWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCrosshairWidget> CrosshairWidget;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;
};
