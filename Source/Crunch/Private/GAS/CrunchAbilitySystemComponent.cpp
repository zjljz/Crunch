// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CrunchAbilitySystemComponent.h"

#include "GAS/CrunchAttributeSet.h"

UCrunchAbilitySystemComponent::UCrunchAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCrunchAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

	GenericConfirmInputID = (int32)ECrunchAbilityInputID::Confirm;
	GenericCancelInputID = (int32)ECrunchAbilityInputID::Cancel;
}

void UCrunchAbilitySystemComponent::InitBaseAttribute()
{
	
}

void UCrunchAbilitySystemComponent::ServerSideInit()
{
	InitBaseAttribute();
	GiveInitialAbilities();
	ApplyInitialEffects();
}

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

void UCrunchAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> ApplyGE, int32 Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle GESpecHandle = MakeOutgoingSpec(ApplyGE, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*GESpecHandle.Data.Get());
	}
}

void UCrunchAbilitySystemComponent::ApplyFullStat()
{
	AuthApplyGameplayEffect(FullStatEffect);
}

void UCrunchAbilitySystemComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!GetOwner())
	{
		return;
	}

	if (Data.NewValue <= 0.0f)
	{
		AuthApplyGameplayEffect(DeathEffect);
	}
}
