// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CrunchAbilitySystemStatics.h"

#include "AbilitySystemComponent.h"
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

	CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
	return FMath::Abs(Cost);
}

bool UCrunchAbilitySystemStatics::IsActorHaveTag(const AActor* ActorToCheck, const FGameplayTag& Tag)
{
	if (UAbilitySystemComponent* ActorASC = ActorToCheck->GetComponentByClass<UAbilitySystemComponent>())
	{
		return ActorASC->HasMatchingGameplayTag(Tag);
	}

	return false;
}

bool UCrunchAbilitySystemStatics::IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec)
{
	//@todo: 这里写死Ability最高等级为4级, 可以考虑数据驱动.
	return Spec.Level >= 4;
}

bool UCrunchAbilitySystemStatics::CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec, const UAbilitySystemComponent& ASC)
{
	if (const UGameplayAbility* AbilityCDO = AbilitySpec.Ability)
	{
		return AbilityCDO->CheckCost(AbilitySpec.Handle, ASC.AbilityActorInfo.Get());
	}

	return false;
}

bool UCrunchAbilitySystemStatics::CheckAbilityCostStatic(const UGameplayAbility* Ability, const UAbilitySystemComponent& ASC)
{
	if (Ability)
	{
		return Ability->CheckCost(FGameplayAbilitySpecHandle(), ASC.AbilityActorInfo.Get());
	}

	return false;
}

float UCrunchAbilitySystemStatics::GetManaCostForAbility(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float ManaCost = 0.f;
	if (AbilityCDO)
	{
		if (UGameplayEffect* CostEffect = AbilityCDO->GetCostGameplayEffect())
		{
			FGameplayEffectSpecHandle Spec = ASC.MakeOutgoingSpec(CostEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CostEffect->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(*Spec.Data.Get(), ManaCost);
		}
	}

	return FMath::Abs(ManaCost);
}

float UCrunchAbilitySystemStatics::GetCooldownDurationForAbility(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float CooldownDuration = 0.f;
	if (AbilityCDO)
	{
		if (UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect())
		{
			FGameplayEffectSpecHandle Spec = ASC.MakeOutgoingSpec(CooldownEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CooldownEffect->DurationMagnitude.AttemptCalculateMagnitude(*Spec.Data.Get(), CooldownDuration);
		}
	}

	return FMath::Abs(CooldownDuration);
}

float UCrunchAbilitySystemStatics::GetCooldownRemainingForAbility(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC)
{
	float CooldownRemaining = 0.f;
	if (!AbilityCDO) return 0.f;

	UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
	if (!CooldownEffect) return 0.f;

	FGameplayEffectQuery Query;
	Query.EffectDefinition = CooldownEffect->GetClass();

	TArray<float> FloatArr = ASC.GetActiveEffectsTimeRemaining(Query);
	for (float Remaining : FloatArr)
	{
		if (Remaining > CooldownRemaining)
		{
			CooldownRemaining = Remaining;
		}
	}
	return CooldownRemaining;
}
