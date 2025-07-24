// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "AbilityListView.generated.h"

struct FAbilityWidgetDataRow;
class UGameplayAbility;
enum class ECrunchAbilityInputID : uint8;

/**
 * 
 */
UCLASS()
class CRUNCH_API UAbilityListView : public UListView
{
	GENERATED_BODY()

public:
	void ConfigureAbilities(const TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities);

	const FAbilityWidgetDataRow* GetAbilityWidgetDataForAbility(const TSubclassOf<UGameplayAbility>& AbilityClass) const;

	void AbilityGaugeGenerated(UUserWidget& Widget);

private:
	UPROPERTY(EditAnywhere, Category = "Data")
	TObjectPtr<UDataTable> DT_AbilityWidgetData;
};
