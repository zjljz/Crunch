// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CrunchGameInstance.h"

#include "HttpModule.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/IHttpRequest.h"
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
		UE_LOG(LogTemp, Warning, TEXT("----- Session Create Falied And will Exit !------"))
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
}
