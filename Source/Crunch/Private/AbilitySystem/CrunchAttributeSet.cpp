// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/AbilitySystem/CrunchAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UCrunchAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCrunchAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UCrunchAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
}

void UCrunchAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		SetCachedHealthPercent(GetHealth() / GetMaxHealth());
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
		SetCachedManaPercent(GetMana() / GetMaxMana());
	}
}

void UCrunchAttributeSet::RescaleHealth()
{
	if (!GetOwningActor()->HasAuthority()) return;

	if (GetCachedHealthPercent() != 0 && GetHealth() != 0)
	{
		SetHealth(GetCachedHealthPercent() * GetMaxHealth());
	}
}

void UCrunchAttributeSet::RescaleMana()
{
	if (!GetOwningActor()->HasAuthority()) return;

	if (GetCachedManaPercent() != 0 && GetMana() != 0)
	{
		SetMana(GetCachedManaPercent() * GetMaxMana());
	}
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

void UCrunchAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchAttributeSet, AttackDamage, OldVal);
}

void UCrunchAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchAttributeSet, Armor, OldVal);
}

void UCrunchAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldVal)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCrunchAttributeSet, MoveSpeed, OldVal);
}
