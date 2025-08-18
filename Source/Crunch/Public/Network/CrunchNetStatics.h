// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrunchNetStatics.generated.h"


UCLASS()
class UCrunchNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static uint8 GetPlayerCountPerTeam();
};
