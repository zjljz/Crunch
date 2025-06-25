// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadStatsGauge.generated.h"

class UValueGauge;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class CRUNCH_API UOverHeadStatsGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	void ConfigureWithASC(UAbilitySystemComponent* ASC);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> HealthGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UValueGauge> ManaGauge;
};
