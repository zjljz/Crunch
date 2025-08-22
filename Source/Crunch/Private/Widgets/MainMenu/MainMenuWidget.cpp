// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu/MainMenuWidget.h"
#include "Framework/CrunchGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/WidgetSwitcher.h"
#include "Widgets/MainMenu/WaitingWidget.h"


void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CrunchGameInstance = GetGameInstance<UCrunchGameInstance>();
	if (CrunchGameInstance)
	{
		CrunchGameInstance->OnLoginCompletedDelegate.AddUObject(this, &ThisClass::LoginCompleted);
		if (CrunchGameInstance->IsLoggedIn())
		{
			SwitchToMainMenuWidget();
		}
	}

	if (Btn_Login)
	{
		Btn_Login->OnClicked.AddDynamic(this, &ThisClass::OnLoginBtnClicked);
	}

	if (Btn_CreateSession && EditText_NewSessionName)
	{
		Btn_CreateSession->OnClicked.AddDynamic(this, &ThisClass::OnCreateSessionBtnClicked);
		Btn_CreateSession->SetIsEnabled(false);
		
		EditText_NewSessionName->OnTextChanged.AddDynamic(this, &ThisClass::OnNewSessionNameChanged);
	}
}

void UMainMenuWidget::OnLoginBtnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("LoginBtn Click and Try Login !"));

	if (CrunchGameInstance && !CrunchGameInstance->IsLoggingIn() && !CrunchGameInstance->IsLoggedIn())
	{
		CrunchGameInstance->ClientAccountPortalLogin();
		SwitchToWaitingWidget(FText::FromString(TEXT("Logging In !")));
	}
}

void UMainMenuWidget::LoginCompleted(bool bSuccessful, const FString& PlayerNickName, const FString& ErrorMsg)
{
	if (bSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Logging Successful ! ! !"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Logging Failed ! ! !"));
	}

	SwitchToMainMenuWidget();
}

FOnButtonClickedEvent& UMainMenuWidget::SwitchToWaitingWidget(const FText& InWaitInfo, bool bAllowCancel)
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->SetWaitInfo(InWaitInfo, bAllowCancel);
	return WaitingWidget->ClearAndGetButtonClickedEvent();
}

void UMainMenuWidget::SwitchToMainMenuWidget() const
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
	}
}

void UMainMenuWidget::OnCreateSessionBtnClicked()
{
	if (CrunchGameInstance && CrunchGameInstance->IsLoggedIn())
	{
		CrunchGameInstance->RequestCreateAndJoinSession(FName(EditText_NewSessionName->GetText().ToString()));
		SwitchToWaitingWidget(FText::FromString(TEXT("Creating Lobby ~ ~ ~")), true).AddDynamic(this, &ThisClass::CancelSessionCreation);
	}
}

void UMainMenuWidget::OnNewSessionNameChanged(const FText& NewText)
{
	Btn_CreateSession->SetIsEnabled(!NewText.IsEmpty());
}

void UMainMenuWidget::CancelSessionCreation()
{
	if (CrunchGameInstance)
	{
		CrunchGameInstance->CancelSessionCreation();
	}
	SwitchToMainMenuWidget();
}
