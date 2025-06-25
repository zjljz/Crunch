// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CrunchAbilitySystemStatics.h"


FGameplayTag UCrunchAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack");
}