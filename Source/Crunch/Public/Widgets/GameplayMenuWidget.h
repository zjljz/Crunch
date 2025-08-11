// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameplayMenuWidget.generated.h"

class FOnButtonClickedEvent;
class UTextBlock;
/**
 * 
 */
UCLASS()
class CRUNCH_API UGameplayMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FOnButtonClickedEvent& GetResumeButtonClickEvent() const;

	void SetMenuTitleString(const FString& NewTitle) const;
private:
	UFUNCTION()
	void BackToMainMenu();

	UFUNCTION()
	void QuitGame();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_MenuTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Resume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_MainMenu;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_QuitGame;
};
