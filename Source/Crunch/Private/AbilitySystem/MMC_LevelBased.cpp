// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC_LevelBased.h"

#include "AbilitySystem/CrunchHeroAttributeSet.h"

UMMC_LevelBased::UMMC_LevelBased()
{
	LevelCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LevelCaptureDef.AttributeToCapture = UCrunchHeroAttributeSet::GetLevelAttribute();

	RelevantAttributesToCapture.Add(LevelCaptureDef);
}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float Ret = 0.f;

	UAbilitySystemComponent* ASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();

	float Level = 0.f;
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	GetCapturedAttributeMagnitude(LevelCaptureDef, Spec, Params, Level);

	bool bFound = false;
	float RateAttributeVal = ASC->GetGameplayAttributeValue(RateAttribute, bFound);
	
	
	return (Level - 1) * RateAttributeVal;
}
