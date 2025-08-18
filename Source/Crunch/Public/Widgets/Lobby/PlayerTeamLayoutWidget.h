// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "PlayerTeamLayoutWidget.generated.h"

class UHorizontalBox;
class UPlayerTeamSlotWidget;
/**
 * 
 */
UCLASS()
class CRUNCH_API UPlayerTeamLayoutWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelectionArr);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> TeamOneLayoutBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> TeamTwoLayoutBox;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<UPlayerTeamSlotWidget> TeamSlotWidgetClass;

	UPROPERTY()
	TArray<UPlayerTeamSlotWidget*> TeamSlotWidgetArray;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float TeamSlotWidgetPaddingMargin = 5.f;
};
