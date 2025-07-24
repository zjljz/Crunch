// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CrunchAbilitySystemStatics.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"


float UCrunchAbilitySystemStatics::GetCooldownDurationForAbility(const UGameplayAbility* Ability)
{
	float CooldownDuration = 0.f;
	const UGameplayEffect* CooldownEffect = Ability->GetCooldownGameplayEffect();
	
	if (!CooldownEffect || !Ability) return CooldownDuration;

	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1.f, CooldownDuration);
	return CooldownDuration;
}

float UCrunchAbilitySystemStatics::GetCostForAbility(const UGameplayAbility* Ability)
{
	float Cost = 0.f;
	const UGameplayEffect* CostEffect = Ability->GetCostGameplayEffect();

	if (!CostEffect || !Ability || CostEffect->Modifiers.Num() == 0) return Cost;

	CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1,Cost);
	return FMath::Abs(Cost);
}
