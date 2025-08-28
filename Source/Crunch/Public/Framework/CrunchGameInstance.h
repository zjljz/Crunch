// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CrunchGameInstance.generated.h"

class FOnlineSessionSearch;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnLoginCompleted, bool /* bWasSuccessful */, const FString& /* PlayerNickName */, const FString& /* ErrorMsg */);
DECLARE_MULTICAST_DELEGATE(FOnJoinSessionFailed);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFindGlobalSessionCompleted, const TArray<FOnlineSessionSearchResult>& /* SearchResults */);

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

	//开始全局Session的寻找.
	void StartGlobalSessionSearch();

	//定时寻找已经创建的Session.
	void StartFindCreatedSession(const FGuid& SessionSearchId);

	//
	bool JoinSessionWithId(const FString& SessionIdStr);
	
private:
	//发送HTTP请求创建CreateSession完成后的回调.
	void OnCreateSessionRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectSuccessfully, FGuid SessionSearchId);

	//停止寻找所有的Session.
	void StopAllSessionFindings();

	//停止寻找本机创建的Session.
	void StopFindCreatedSession();

	//停止搜索全局的Session。
	void StopGlobalSessionSearch();

	//寻找本机创建的Session.
	void FindCreatedSession(FGuid SessionSearchId);

	//寻找本机创建Session超时.
	void OnFindCreatedSessionTimeOut();

	//寻找本机创建Session成功的回调.
	void OnFindCreatedSessionComplete(bool bWasSuccessful);

	//加入Session.
	void JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult);

	//JoinSession 完成时的回调.
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port);

	void FindGlobalSessions();

	void OnFindGlobalSessionComplete(bool bWasSuccessful);

public:
	FOnLoginCompleted OnLoginCompletedDelegate;

	FOnJoinSessionFailed OnJoinSessionFailed;

	FOnFindGlobalSessionCompleted OnFindGlobalSessionCompleted;

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

	//在寻找已创建的Session时 需要的定时器.
	FTimerHandle FindCreatedSessionTimerHandle;
	FTimerHandle FindCreatedSessionTimeOutHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float FindCreatedSessionSearchInterval = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float FindCreatedSessionTimeOutDuration = 60.f;


	FTimerHandle GlobalSessionSearchHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float GlobalSessionSearchInterval = 2.f;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
