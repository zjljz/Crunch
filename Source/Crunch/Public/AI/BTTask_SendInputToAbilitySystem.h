// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Crunch/Public/AbilitySystem/CrunchGameplayAbilityTypes.h"
#include "BTTask_SendInputToAbilitySystem.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UBTTask_SendInputToAbilitySystem : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SendInputToAbilitySystem();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	UPROPERTY(EditAnywhere, Category = "Ability")
	ECrunchAbilityInputID TaskInputID = ECrunchAbilityInputID::None;
};
