// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class USessionEntryWidget;
class UScrollBox;
class UEditableText;
class FOnButtonClickedEvent;
class UWaitingWidget;
class UCrunchGameInstance;
class UButton;
class UWidgetSwitcher;

/**
 * 
 */
UCLASS()
class CRUNCH_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnLoginBtnClicked();

	void LoginCompleted(bool bSuccessful, const FString& PlayerNickName, const FString& ErrorMsg);

	FOnButtonClickedEvent& SwitchToWaitingWidget(const FText& InWaitInfo, bool bAllowCancel = false);

	void SwitchToMainMenuWidget() const;

	UFUNCTION()
	void OnCreateSessionBtnClicked();

	UFUNCTION()
	void OnNewSessionNameChanged(const FText& NewText);

	//当CreateSession时 WaitingWidget的Btn 绑定的取消CreateSession.
	UFUNCTION()
	void CancelSessionCreation();

	void OnJoinSessionFailed();

	//当寻找网络上的会话成功时 更新会话列表.
	void UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& SearchResults);

	UFUNCTION()
	void OnJoinSessionBtnClicked();

	void OnSessionEntrySelected(const FString& SelectedEntryIdStr);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MainSwitcher;

	UPROPERTY()
	TObjectPtr<UCrunchGameInstance> CrunchGameInstance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> LoginWidgetRoot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> MainWidgetRoot;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Login;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWaitingWidget> WaitingWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_CreateSession;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> EditText_NewSessionName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> SessionScrollBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_JoinSession;
	
	UPROPERTY(EditDefaultsOnly, Category = "Session")
	TSubclassOf<USessionEntryWidget> SessionEntryWidgetClass;

	//记录当前选择的SessionId.
	FString CurSelectedSessionId = "";
};
