// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionEntryWidget.generated.h"

class UButton;
class UTextBlock;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionEntrySelected, const FString& /* SelectedSessionIdStr */)

/**
 * 
 */
UCLASS()
class CRUNCH_API USessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void InitializeEntry(const FString& SessionName, const FString& SessionIdStr);

	FORCEINLINE FString GetCachedSessionIdStr() const { return CachedSessionIdStr; }

private:
	UFUNCTION()
	void OnSessionBtnClicked();

public:
	FOnSessionEntrySelected OnSessionEntrySelected;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SessionName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Session;

	FString CachedSessionIdStr;
};
