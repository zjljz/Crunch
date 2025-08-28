// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "CrunchNetStatics.generated.h"


UCLASS()
class UCrunchNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static IOnlineSessionPtr GetSessionPtr();
	static IOnlineIdentityPtr GetIdentityPtr();
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& ServerSessionName, const FString& ServerSessionSearchId, int ServerSessionPort);
	
	static uint8 GetPlayerCountPerTeam();

	static bool IsSessionServer(const UObject* WorldContextObject);

	static FString GetCommandLineArgAsString(const FName& ParamName);

	static int GetCommandLineArgAsInt(const FName& ParamName);

	static FString GetSessionNameStr();
	static FName GetSessionNameKey();

	static FString GetSessionSearchIdStr();
	static FName GetSessionSearchIdKey();

	static int GetSessionPort();
	static FName GetPortKey();

	static FName GetCoordinatorURLKey();
	static FString GetCoordinatorURL();
	static FString GetDefaultCoordinatorURL();

	static FString GetTestURL();
	static FName GetTestURLKey();

	static void ReplacePort(FString& OutURLStr, int NewPort);
};
