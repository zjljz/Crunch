// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitingWidget.generated.h"

class UButton;
class FOnButtonClickedEvent;
class UTextBlock;
/**
 * 
 */
UCLASS()
class CRUNCH_API UWaitingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FOnButtonClickedEvent& ClearAndGetButtonClickedEvent() const;

	void SetWaitInfo(const FText& InText, bool bAllowCancel = true);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Cancel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_WaitInfo;
};
