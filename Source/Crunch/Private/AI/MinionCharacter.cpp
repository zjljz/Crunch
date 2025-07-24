// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MinionCharacter.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

AMinionCharacter::AMinionCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMinionCharacter::GetSkinBasedOnTeamId()
{
	USkeletalMesh** Skin = SkinMap.Find(GetGenericTeamId());
	if (Skin)
	{
		GetMesh()->SetSkeletalMesh(*Skin);
	}
}

void AMinionCharacter::SetGoal(AActor* NewGoal)
{
	if (AAIController* AIController = GetController<AAIController>())
	{
		if (UBlackboardComponent* BBComp = AIController->GetBlackboardComponent())
		{
			BBComp->SetValueAsObject(BB_GoalKeyName, NewGoal);
		}
	}
}

void AMinionCharacter::OnRep_TeamId()
{
	Super::OnRep_TeamId();

	GetSkinBasedOnTeamId();
}

void AMinionCharacter::OnDeath()
{
}

void AMinionCharacter::OnReSpawn()
{
}
