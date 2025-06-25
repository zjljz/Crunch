// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CrunchAbilitySystemComponent.h"

void UCrunchAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	for (auto GEClass : InitialEffects)
	{
		if (GEClass)
		{
			FGameplayEffectContextHandle GEContextHandle = MakeEffectContext();
			FGameplayEffectSpecHandle GESpec = MakeOutgoingSpec(GEClass, 1.0f, GEContextHandle);
			ApplyGameplayEffectSpecToSelf(*GESpec.Data.Get());
		}
	}
}

void UCrunchAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	for (auto AbilityPair : BaseAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, (int32)AbilityPair.Key, nullptr));
	}

	for (auto AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr));
	}
}
