// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainMenu/SessionEntryWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"


void USessionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Session->OnClicked.AddDynamic(this, &ThisClass::OnSessionBtnClicked);
}

void USessionEntryWidget::InitializeEntry(const FString& SessionName, const FString& SessionIdStr)
{
	Text_SessionName->SetText(FText::FromString(SessionName));
	CachedSessionIdStr = SessionIdStr;
}

void USessionEntryWidget::OnSessionBtnClicked()
{
	OnSessionEntrySelected.Broadcast(CachedSessionIdStr);
}