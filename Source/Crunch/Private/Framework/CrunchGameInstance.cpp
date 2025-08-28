// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CrunchGameInstance.h"

#include "HttpModule.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Network/CrunchNetStatics.h"

void UCrunchGameInstance::Init()
{
	Super::Init();

	if (GetWorld()->IsEditorWorld()) return;

	if (UCrunchNetStatics::IsSessionServer(this))
	{
		CreateSession();
	}
}

void UCrunchGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == NM_DedicatedServer || GetWorld()->GetNetMode() == NM_ListenServer)
	{
		LoadLevelAndListen(GameLevel);
	}
}

void UCrunchGameInstance::OnPlayerJoined(const FUniqueNetIdRepl& UniqueNetId)
{
	if (WaitPlayerJoinTimerOutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimerOutHandle);
	}

	PlayerRecord.Add(UniqueNetId);
}

void UCrunchGameInstance::OnPlayerLeft(const FUniqueNetIdRepl& UniqueNetId)
{
	PlayerRecord.Remove(UniqueNetId);

	if (PlayerRecord.IsEmpty() && !WaitPlayerJoinTimerOutHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("All Player left the Session, terminating ! ! ! "));
		TerminateSessionServer();
	}
}

void UCrunchGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(FPackageName::ObjectPathToObjectName(Level.ToString()));

	if (LevelURL != "")
	{
		FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), ServerSessionPort);
		UE_LOG(LogTemp, Warning, TEXT("Server Travel To : %s"), *TravelStr);
		GetWorld()->ServerTravel(TravelStr);
	}
}

void UCrunchGameInstance::CreateSession()
{
	if (IOnlineSessionPtr SessionPtr = UCrunchNetStatics::GetSessionPtr())
	{
		ServerSessionName = UCrunchNetStatics::GetSessionNameStr();
		FString SessionSearchId = UCrunchNetStatics::GetSessionSearchIdStr();
		ServerSessionPort = UCrunchNetStatics::GetSessionPort();

		FOnlineSessionSettings Settings = UCrunchNetStatics::GenerateOnlineSessionSettings(FName(ServerSessionName), SessionSearchId, ServerSessionPort);
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnCreateSessionComplete);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), Settings))
		{
			UE_LOG(LogTemp, Error, TEXT("Session Creat Failed Right Away ! ! !"));
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("### CreateSession With Name : %s, Id : %s, Port : %d ###"), *ServerSessionName, *SessionSearchId, ServerSessionPort);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Session ptr, Terminating ! ! !"));
		TerminateSessionServer();
	}
}

void UCrunchGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT(" ----------- Session Created ! --------"));
		GetWorld()->GetTimerManager().SetTimer(WaitPlayerJoinTimerOutHandle, this, &UCrunchGameInstance::OnPlayerJoinTimeOut, WaitPlayerJoinTimeOutCount);
		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("----- Session Create Failed And will Exit !------"))
		TerminateSessionServer();
	}

	if (IOnlineSessionPtr OnlineSessionPtr = UCrunchNetStatics::GetSessionPtr())
	{
		OnlineSessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCrunchGameInstance::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UCrunchGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr OnlineSessionPtr = UCrunchNetStatics::GetSessionPtr())
	{
		OnlineSessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		OnlineSessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &ThisClass::OnEndSessionComplete);
		if (!OnlineSessionPtr->EndSession(FName(ServerSessionName)))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UCrunchGameInstance::OnPlayerJoinTimeOut()
{
	UE_LOG(LogTemp, Error, TEXT("Server Session shut down after %f seconds with out player joining"), WaitPlayerJoinTimeOutCount);
	GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimerOutHandle);
	TerminateSessionServer();
}

bool UCrunchGameInstance::IsLoggedIn() const
{
	if (IOnlineIdentityPtr IdentityPtr = UCrunchNetStatics::GetIdentityPtr())
	{
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}

	return false;
}

bool UCrunchGameInstance::IsLoggingIn() const
{
	return LoggingInDelegateHandle.IsValid();
}

void UCrunchGameInstance::ClientAccountPortalLogin()
{
	ClientLogin("AccountPortal", "", "");
}

void UCrunchGameInstance::ClientLogin(const FString& Type, const FString& Id, const FString& Token)
{
	if (IOnlineIdentityPtr IdentityPtr = UCrunchNetStatics::GetIdentityPtr())
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}
		LoggingInDelegateHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &ThisClass::OnLoginCompleted);

		if (!IdentityPtr->Login(0, FOnlineAccountCredentials(Type, Id, Token)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Login Failed Right Away !"));

			if (LoggingInDelegateHandle.IsValid())
			{
				IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
				LoggingInDelegateHandle.Reset();
			}
			OnLoginCompletedDelegate.Broadcast(false, "", "Login Failed !");
		}
	}
}

void UCrunchGameInstance::OnLoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorMsg)
{
	if (IOnlineIdentityPtr IdentityPtr = UCrunchNetStatics::GetIdentityPtr())
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}

		FString PlayerNickName = "";
		if (bWasSuccessful)
		{
			PlayerNickName = IdentityPtr->GetPlayerNickname(UserId);
			UE_LOG(LogTemp, Warning, TEXT("Logged Successfully as : %s"), *PlayerNickName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Logging failed as : %s"), *ErrorMsg);
		}

		OnLoginCompletedDelegate.Broadcast(bWasSuccessful, PlayerNickName, ErrorMsg);
	}
	else
	{
		OnLoginCompletedDelegate.Broadcast(false, "", "Can't Find Identity Ptr !");
	}
}

void UCrunchGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Request Create Session for Session Name : %s"), *NewSessionName.ToString());

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FGuid SessionSearchId = FGuid::NewGuid();

	FString CoordinatorURL = UCrunchNetStatics::GetCoordinatorURL();
	UE_LOG(LogTemp, Warning, TEXT("Send Request Creation to URL : %s"), *CoordinatorURL);

	FString URL = FString::Printf(TEXT("%s/Sessions"), *CoordinatorURL);

	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(UCrunchNetStatics::GetSessionNameKey().ToString(), NewSessionName.ToString());
	JsonObject->SetStringField(UCrunchNetStatics::GetSessionSearchIdKey().ToString(), SessionSearchId.ToString());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnCreateSessionRequestCompleted, SessionSearchId);

	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Error, TEXT("Session Creation Request Failed Right Away ! ! !"));
	}
}

void UCrunchGameInstance::CancelSessionCreation()
{
	UE_LOG(LogTemp, Warning, TEXT("Cancel Create Session !"));
	StopAllSessionFindings();

	StartGlobalSessionSearch();
}

void UCrunchGameInstance::StartGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Global Session Search!"));

	GetWorld()->GetTimerManager().SetTimer(GlobalSessionSearchHandle, this, &ThisClass::FindGlobalSessions, GlobalSessionSearchInterval, true, 0.f);
}

void UCrunchGameInstance::StartFindCreatedSession(const FGuid& SessionSearchId)
{
	if (!SessionSearchId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session Search Id is Invalid, Can't Start find session."))
		return;
	}

	StopAllSessionFindings();

	UE_LOG(LogTemp, Warning, TEXT("Start Find the Created Session with Id : %s"), *SessionSearchId.ToString());

	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::FindCreatedSession, SessionSearchId),
	                                       FindCreatedSessionSearchInterval, true, 0.f);

	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimeOutHandle, this, &ThisClass::OnFindCreatedSessionTimeOut, FindCreatedSessionTimeOutDuration);
}

bool UCrunchGameInstance::JoinSessionWithId(const FString& SessionIdStr)
{
	if (SessionSearch.IsValid())
	{
		const FOnlineSessionSearchResult* Result = SessionSearch->SearchResults.FindByPredicate([=](const FOnlineSessionSearchResult& Cur)
		{
			return Cur.GetSessionIdStr() == SessionIdStr;
		});

		if (Result)
		{
			JoinSessionWithSearchResult(*Result);
			return true;
		}
	}

	return false;
}

void UCrunchGameInstance::OnCreateSessionRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectSuccessfully, FGuid SessionSearchId)
{
	if (!bConnectSuccessfully)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't Connect to Server"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Create Session Request Completed ~ ~ ~"));
	}

	int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Error, TEXT("Session Creation Failed with code : %d"), ResponseCode);
		return;
	}

	FString ResponseStr = Response->GetContentAsString();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseStr);
	int32 Port = 0;

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		Port = JsonObject->GetIntegerField(*UCrunchNetStatics::GetPortKey().ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("Connected to Coordinator Successfully and the new Session created is on port : %d"), Port);
	StartFindCreatedSession(SessionSearchId);
}

void UCrunchGameInstance::StopAllSessionFindings()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop All Session Search"));
	StopFindCreatedSession();
	StopGlobalSessionSearch();
}

void UCrunchGameInstance::StopFindCreatedSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Find Created Session"));
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimeOutHandle);

	if (IOnlineSessionPtr SessionPtr = UCrunchNetStatics::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCrunchGameInstance::StopGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Global Session Search"));

	if (GlobalSessionSearchHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GlobalSessionSearchHandle);
	}

	if (IOnlineSessionPtr SessionPtr = UCrunchNetStatics::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UCrunchGameInstance::FindCreatedSession(FGuid SessionSearchId)
{
	UE_LOG(LogTemp, Warning, TEXT("Try to find Created Session for SearchId : %s"), *SessionSearchId.ToString())

	IOnlineSessionPtr SessionPtr = UCrunchNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Session Ptr,and Cancel Session Search"));
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	if (!SessionSearch)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't Create Session Search, and Cancel Session Search"));
		return;
	}

	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 1;
	SessionSearch->QuerySettings.Set(UCrunchNetStatics::GetSessionSearchIdKey(), SessionSearchId.ToString(), EOnlineComparisonOp::Equals);

	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::OnFindCreatedSessionComplete);
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Error, TEXT("Find Session Failed Right Away..."))
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UCrunchGameInstance::OnFindCreatedSessionTimeOut()
{
	UE_LOG(LogTemp, Error, TEXT("Try find Created Session time out ! ! !"))
	StopFindCreatedSession();
}

void UCrunchGameInstance::OnFindCreatedSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful || SessionSearch->SearchResults.Num() == 0)
	{
		return;
	}

	StopFindCreatedSession();
	JoinSessionWithSearchResult(SessionSearch->SearchResults[0]);
}

void UCrunchGameInstance::JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Join Session with SearchResult"));

	IOnlineSessionPtr SessionPtr = UCrunchNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't Find Online Session Ptr"));
		return;
	}

	FString SessionName = "";
	SearchResult.Session.SessionSettings.Get<FString>(UCrunchNetStatics::GetSessionNameKey(), SessionName);

	const FOnlineSessionSetting* PortSetting = SearchResult.Session.SessionSettings.Settings.Find(UCrunchNetStatics::GetPortKey());
	int64 Port = 7777;
	PortSetting->Data.GetValue(Port);

	UE_LOG(LogTemp, Warning, TEXT("Trying to join session :%s, at port : %lld"), *SessionName, Port);

	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &ThisClass::OnJoinSessionComplete, (int)Port);
	if (!SessionPtr->JoinSession(0, FName(SessionName), SearchResult))
	{
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		UE_LOG(LogTemp, Error, TEXT("Join Session Failed Right Away !"));
		OnJoinSessionFailed.Broadcast();
	}
}

void UCrunchGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port)
{
	IOnlineSessionPtr SessionPtr = UCrunchNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Join Session with no Session Ptr."))
		OnJoinSessionFailed.Broadcast();
		return;
	}

	if (JoinResult == EOnJoinSessionCompleteResult::Success)
	{
		StopAllSessionFindings();
		UE_LOG(LogTemp, Warning, TEXT("Join Session : %s Successful, the port is : %d"), *SessionName.ToString(), Port);

		FString TravelURL = "";
		SessionPtr->GetResolvedConnectString(SessionName, TravelURL);

		FString TestURL = UCrunchNetStatics::GetTestURL();
		if (!TestURL.IsEmpty())
		{
			TravelURL = TestURL;
		}

		UCrunchNetStatics::ReplacePort(TravelURL, Port);

		UE_LOG(LogTemp, Warning, TEXT("Traveling to Session at : %s"), *TravelURL);

		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, TRAVEL_Absolute);
	}
	else
	{
		OnJoinSessionFailed.Broadcast();
	}

	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
}

void UCrunchGameInstance::FindGlobalSessions()
{
	UE_LOG(LogTemp, Warning, TEXT("------- Trying Find Global Session ---------"));

	IOnlineSessionPtr SessionPtr = UCrunchNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Session Ptr, and Cancel Session Search"));
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 20;

	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::OnFindGlobalSessionComplete);

	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Error, TEXT("Find Global Session failed right Away !"));
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UCrunchGameInstance::OnFindGlobalSessionComplete(bool bWasSuccessful)
{
	IOnlineSessionPtr SessionPtr = UCrunchNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Session Ptr when find global session completed"));
		return;
	}

	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);

	if (bWasSuccessful)
	{
		OnFindGlobalSessionCompleted.Broadcast(SessionSearch->SearchResults);

		for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
		{
			FString SessionName = "Name_None";
			Result.Session.SessionSettings.Get<FString>(UCrunchNetStatics::GetSessionNameKey(), SessionName);
			UE_LOG(LogTemp, Warning, TEXT("Found Session : %s when Global Session Search."), *SessionName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find Sessions when Global Session Search ! ! !"));
	}
}
