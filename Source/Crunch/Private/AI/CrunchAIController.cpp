// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CrunchAIController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/CrunchCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ACrunchAIController::ACrunchAIController()
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;

	SightConfig->SetMaxAge(5.f);

	SightConfig->PeripheralVisionAngleDegrees = 180.f;

	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ACrunchAIController::OnTargetPerceptionUpdated);
	AIPerceptionComp->OnTargetPerceptionForgotten.AddDynamic(this, &ACrunchAIController::OnTargetPerceptionForgotten);
}

void ACrunchAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (ACrunchCharacter* CrunchCharacter = Cast<ACrunchCharacter>(InPawn))
	{
		SetGenericTeamId(CrunchCharacter->GetGenericTeamId());
		ClearAndDisableAllSenses();
		EnableAllSenses();
	}

	// 为ASC绑定DeadTag事件.
	if (UAbilitySystemComponent* PawnASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InPawn))
	{
		FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("Stats.Dead"));
		PawnASC->RegisterGameplayTagEvent(DeadTag).AddUObject(this, &ThisClass::PawnDeadTagUpdate);
	}
}

void ACrunchAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);
}

void ACrunchAIController::OnTargetPerceptionUpdated(AActor* Target, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(Target);
		}
	}
	else
	{
		ForgetActorIfDead(Target);
	}
}

void ACrunchAIController::OnTargetPerceptionForgotten(AActor* Target)
{
	if (!Target)
	{
		return;
	}

	// If the current target is forgotten, we need to set the next target
	if (GetCurrentTarget() == Target)
	{
		SetCurrentTarget(GetNextPerceiveActor());
	}
}

UObject* ACrunchAIController::GetCurrentTarget() const
{
	if (!GetBlackboardComponent())
	{
		return nullptr;
	}

	return GetBlackboardComponent()->GetValueAsObject(BB_TargetKeyName);
}

void ACrunchAIController::SetCurrentTarget(AActor* NewTarget)
{
	if (!GetBlackboardComponent())
	{
		return;
	}

	if (NewTarget)
	{
		GetBlackboardComponent()->SetValueAsObject(BB_TargetKeyName, NewTarget);
	}
	else
	{
		GetBlackboardComponent()->ClearValue(BB_TargetKeyName);
	}
}

AActor* ACrunchAIController::GetNextPerceiveActor() const
{
	if (PerceptionComponent)
	{
		TArray<AActor*> Actors;
		AIPerceptionComp->GetPerceivedHostileActors(Actors);

		if (Actors.Num() > 0)
		{
			return Actors[0];
		}
	}

	return nullptr;
}

void ACrunchAIController::ForgetActorIfDead(AActor* Target)
{
	if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target))
	{
		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Stats.Dead"))))
		{
			for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator Iter = AIPerceptionComp->GetPerceptualDataIterator(); Iter; ++Iter)
			{
				if (Iter.Key() != Target)
				{
					continue;
				}

				for (FAIStimulus& Stimulus : Iter.Value().LastSensedStimuli)
				{
					Stimulus.SetStimulusAge(TNumericLimits<float>::Max());
				}
			}
		}
	}
}

void ACrunchAIController::ClearAndDisableAllSenses()
{
	AIPerceptionComp->AgeStimuli(TNumericLimits<float>::Max());

	for (auto SenseConfigIt = AIPerceptionComp->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComp->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), false);
	}

	if (GetBlackboardComponent())
	{
		GetBlackboardComponent()->ClearValue(BB_TargetKeyName);
	}
}

void ACrunchAIController::EnableAllSenses()
{
	for (auto SenseConfigIt = AIPerceptionComp->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComp->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), true);
	}
}

void ACrunchAIController::PawnDeadTagUpdate(FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		GetBrainComponent()->StopLogic(TEXT("Pawn is dead"));
		ClearAndDisableAllSenses();
	}
	else
	{
		GetBrainComponent()->StartLogic();
		EnableAllSenses();
	}
}
