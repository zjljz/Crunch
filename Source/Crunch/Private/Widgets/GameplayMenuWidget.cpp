// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayMenuWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameplayMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_MainMenu->OnClicked.AddDynamic(this, &ThisClass::BackToMainMenu);
	Btn_QuitGame->OnClicked.AddDynamic(this, &ThisClass::QuitGame);
}

FOnButtonClickedEvent& UGameplayMenuWidget::GetResumeButtonClickEvent() const
{
	return Btn_Resume->OnClicked;
}

void UGameplayMenuWidget::SetMenuTitleString(const FString& NewTitle) const
{
	Text_MenuTitle->SetText(FText::FromString(NewTitle));
}

void UGameplayMenuWidget::BackToMainMenu()
{
}

void UGameplayMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
