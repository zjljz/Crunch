// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/AbilitySystem/MMC_BaseAttackDamage.h"

#include "Crunch/Public/AbilitySystem/CrunchAttributeSet.h"

UMMC_BaseAttackDamage::UMMC_BaseAttackDamage()
{
	DamageCaptureDef.AttributeToCapture = UCrunchAttributeSet::GetAttackDamageAttribute();
	DamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	ArmorCaptureDef.AttributeToCapture = UCrunchAttributeSet::GetArmorAttribute();
	ArmorCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	RelevantAttributesToCapture.Add(DamageCaptureDef);
	RelevantAttributesToCapture.Add(ArmorCaptureDef);
}

float UMMC_BaseAttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvaParams;

	EvaParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float AttackDamage = 0.f;
	GetCapturedAttributeMagnitude(DamageCaptureDef, Spec, EvaParams, AttackDamage);

	float Armor = 0.f;
	GetCapturedAttributeMagnitude(ArmorCaptureDef, Spec, EvaParams, Armor);

	float Ret = AttackDamage * (1 - Armor / (Armor + 100));

	return -Ret;
}
