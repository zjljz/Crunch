// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Perception/AIPerceptionTypes.h"
#include "CrunchAIController.generated.h"


class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class CRUNCH_API ACrunchAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACrunchAIController();

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Target, FAIStimulus Stimulus);

	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Target);
	
	UObject* GetCurrentTarget() const;
	void SetCurrentTarget(AActor* NewTarget);

	AActor* GetNextPerceiveActor() const;

	void ForgetActorIfDead(AActor* Target);

	// Clears the current target and disables all senses
	void ClearAndDisableAllSenses();

	void EnableAllSenses();

	void PawnDeadTagUpdate(FGameplayTag Tag, int32 NewCount);
private:

	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	// The name of the blackboard key used to store the current target
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	FName BB_TargetKeyName;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
};
