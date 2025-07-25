// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/AbilitySystem/CrunchHeroAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UCrunchHeroAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchHeroAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchHeroAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchHeroAttributeSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchHeroAttributeSet, PreLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchHeroAttributeSet, NextLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchHeroAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchHeroAttributeSet, MaxLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchHeroAttributeSet, Gold, COND_None, REPNOTIFY_Always);
}

void UCrunchHeroAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, Intelligence, OldVal);
}

void UCrunchHeroAttributeSet::OnRep_IntelligenceGrowRate(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, IntelligenceGrowRate, OldVal);
}

void UCrunchHeroAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, Strength, OldVal);
}

void UCrunchHeroAttributeSet::OnRep_StrengthGrowRate(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, StrengthGrowRate, OldVal);
}

void UCrunchHeroAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, Experience, OldVal);
}

void UCrunchHeroAttributeSet::OnRep_PreLevelExperience(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, PreLevelExperience, OldVal);
}

void UCrunchHeroAttributeSet::OnRep_NextLevelExperience(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, NextLevelExperience, OldVal);
}

void UCrunchHeroAttributeSet::OnRep_Level(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, Level, OldVal);
}

void UCrunchHeroAttributeSet::OnRep_MaxLevel(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, MaxLevel, OldVal);
}

void UCrunchHeroAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchHeroAttributeSet, Gold, OldVal);
}
