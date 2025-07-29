// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/AbilitySystem/CrunchGameplayAbilityTypes.h"

FGenericDamageEffectDef::FGenericDamageEffectDef() : DamageEffect(nullptr), PushVelocity(FVector::ZeroVector)
{
}

FHeroBaseStats::FHeroBaseStats() : Class(nullptr), Strength(0.f), Intelligence(0.f), StrengthGrowRate(0.f), IntelligenceGrowRate(0.f),
                                   BaseMaxHealth(0.f), BaseMaxMana(0.f), BaseAttackDamage(0.f), BaseArmor(0.f), BaseMoveSpeed(0.f)
{
}
