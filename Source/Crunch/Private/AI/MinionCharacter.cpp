// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MinionCharacter.h"

#include "AbilitySystemComponent.h"

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

bool AMinionCharacter::IsActive() const
{
	FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("Stats.Dead"));
	return !GetAbilitySystemComponent()->HasMatchingGameplayTag(DeadTag);
}

void AMinionCharacter::ActivateMinion()
{
	FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("Stats.Dead"));
	GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(DeadTag));
}

void AMinionCharacter::OnRep_TeamId()
{
	Super::OnRep_TeamId();

	GetSkinBasedOnTeamId();
}


