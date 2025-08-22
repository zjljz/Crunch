// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "CrunchGameSession.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API ACrunchGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	virtual bool ProcessAutoLogin() override;

	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override;
	virtual void UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId) override;
};
