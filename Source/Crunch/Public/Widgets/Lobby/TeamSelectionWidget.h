// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectionWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, uint8 /* SlotId */)

/**
 * 
 */
UCLASS()
class CRUNCH_API UTeamSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FORCEINLINE void SetSlotId(uint8 InSlotId) { SlotId = InSlotId; }

	void UpdateSlotInfo(const FString& PlayerNickName) const;

private:
	UFUNCTION()
	void OnSelectButtonClicked();

public:
	FOnSlotClicked OnSlotClicked;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Select;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Info;

	uint8 SlotId = 0;
};
