// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "CrunchGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnLoginCompleted, bool /* bWasSuccessful */, const FString& /* PlayerNickName */, const FString& /* ErrorMsg */);

/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	void StartMatch();

	void OnPlayerJoined(const FUniqueNetIdRepl& UniqueNetId);
	void OnPlayerLeft(const FUniqueNetIdRepl& UniqueNetId);

private:
	/*********************** Session Server *********************/

	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);

	void CreateSession();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);

	void TerminateSessionServer();

	void OnPlayerJoinTimeOut();


	/********************* Login *********************/

public:
	//是否以及登录.
	bool IsLoggedIn() const;

	//是否正在登录.
	bool IsLoggingIn() const;

	void ClientAccountPortalLogin();

private:
	void ClientLogin(const FString& Type, const FString& Id, const FString& Token);

	void OnLoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorMsg);


	/*********************** Client Session Create and Search **********************/
public:

	//Client申请 创建会话 如果创建成功就加入会话.
	void RequestCreateAndJoinSession(const FName& NewSessionName);

	//取消创建会话
	void CancelSessionCreation();

	//发送HTTP请求创建CreateSession完成后的回调.
	void OnCreateSessionRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectSuccessfully, FGuid SessionSearchId);
	
public:
	FOnLoginCompleted OnLoginCompletedDelegate;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Session")
	float WaitPlayerJoinTimeOutCount = 90.f;

	FString ServerSessionName;
	int ServerSessionPort;

	FTimerHandle WaitPlayerJoinTimerOutHandle;

	TSet<FUniqueNetIdRepl> PlayerRecord;

	//记录的Login委托 在Login完成后会清除 可以通过这个判断是否正在Login.
	FDelegateHandle LoggingInDelegateHandle;
};
