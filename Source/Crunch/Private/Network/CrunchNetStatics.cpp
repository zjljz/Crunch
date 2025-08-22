// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/CrunchNetStatics.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"

IOnlineSessionPtr UCrunchNetStatics::GetSessionPtr()
{
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(nullptr))
	{
		return Subsystem->GetSessionInterface();
	}

	return nullptr;
}

IOnlineIdentityPtr UCrunchNetStatics::GetIdentityPtr()
{
	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(nullptr))
	{
		return Subsystem->GetIdentityInterface();
	}

	return nullptr;
}

FOnlineSessionSettings UCrunchNetStatics::GenerateOnlineSessionSettings(const FName& ServerSessionName, const FString& ServerSessionSearchId, int ServerSessionPort)
{
	FOnlineSessionSettings OnlineSessionSettings{};
	OnlineSessionSettings.bIsLANMatch = false; //是否局域网游戏
	OnlineSessionSettings.NumPublicConnections = GetPlayerCountPerTeam() * 2; //链接数
	OnlineSessionSettings.bShouldAdvertise = true; //是否广告到OnlineServices.
	OnlineSessionSettings.bUsesPresence = false; //显示用户的登录信息 但是因为是服务器创建会话 所以不需要显示.
	OnlineSessionSettings.bAllowJoinViaPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	OnlineSessionSettings.bAllowInvites = true;
	OnlineSessionSettings.bAllowJoinInProgress = false;
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	OnlineSessionSettings.bUsesStats = true;

	OnlineSessionSettings.Set(GetSessionNameKey(), ServerSessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetSessionSearchIdKey(), ServerSessionSearchId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetPortKey(), ServerSessionPort, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	return OnlineSessionSettings;
}

uint8 UCrunchNetStatics::GetPlayerCountPerTeam()
{
	return 5;
}

bool UCrunchNetStatics::IsSessionServer(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetNetMode() == NM_DedicatedServer;
}

FString UCrunchNetStatics::GetCommandLineArgAsString(const FName& ParamName)
{
	FString OutVal = "";

	FString CommandLineArg = FString::Printf(TEXT("%s="), *ParamName.ToString());
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UCrunchNetStatics::GetCommandLineArgAsInt(const FName& ParamName)
{
	int OutVal = 0;

	FString CommandLineArg = FString::Printf(TEXT("%s="), *ParamName.ToString());
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

FString UCrunchNetStatics::GetSessionNameStr()
{
	return GetCommandLineArgAsString(GetSessionNameKey());
}

FName UCrunchNetStatics::GetSessionNameKey()
{
	return FName("SESSION_NAME");
}

FString UCrunchNetStatics::GetSessionSearchIdStr()
{
	return GetCommandLineArgAsString(GetSessionSearchIdKey());
}

FName UCrunchNetStatics::GetSessionSearchIdKey()
{
	return FName("SESSION_SEARCH_ID");
}

int UCrunchNetStatics::GetSessionPort()
{
	return GetCommandLineArgAsInt(GetPortKey());
}

FName UCrunchNetStatics::GetPortKey()
{
	return FName("PORT");
}

FName UCrunchNetStatics::GetCoordinatorURLKey()
{
	return FName("COORDINATOR_URL");
}

FString UCrunchNetStatics::GetCoordinatorURL()
{
	FString Result = GetCommandLineArgAsString(GetCoordinatorURLKey());
	if (Result != "")
	{
		return Result;
	}

	return GetDefaultCoordinatorURL();
}

FString UCrunchNetStatics::GetDefaultCoordinatorURL()
{
	FString CoordinatorURL = "";
	GConfig->GetString(TEXT("Crunch.Net"), TEXT("CoordinatorURL"), CoordinatorURL, GGameIni);
	return CoordinatorURL;
}
