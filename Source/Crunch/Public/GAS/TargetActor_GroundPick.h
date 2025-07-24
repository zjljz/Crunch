// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundPick.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ATargetActor_GroundPick();

	virtual void Tick(float DeltaSeconds) override;

	FVector GetTargetPoint() const;

	virtual void ConfirmTargetingAndContinue() override;

	void SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy = true);

	void SetTargetAreaRadius(float NewRadius);

	FORCEINLINE void SetTraceLineLength(float NewLength)
	{
		TraceLineLength = NewLength;
	}
	
	FORCEINLINE void SetDrawDebug(bool bDraw)
	{
		bDrawDebug = bDraw;
	}
	
private:

	UPROPERTY(VisibleDefaultsOnly, Category = "Visual")
	TObjectPtr<UDecalComponent> DecalComp;
	
	bool bShouldTargetEnemy = true;

	bool bShouldTargetFriendly = false;
	
	float TraceLineLength = 2000.f;
	
	float TargetAreaRadius = 300.f;
	
	bool bDrawDebug = false;
};
