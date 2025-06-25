// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CrunchAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UCrunchAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UCrunchAttributeSet::OnRep_Health(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchAttributeSet, Health, OldVal);
}

void UCrunchAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchAttributeSet, MaxHealth, OldVal);
}

void UCrunchAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchAttributeSet, Mana, OldVal);
}

void UCrunchAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchAttributeSet, MaxMana, OldVal);
}
