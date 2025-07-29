// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemGenericDataAsset.h"

FRealCurve* UAbilitySystemGenericDataAsset::GetExpCurve() const
{
	return ExpCurveTable->FindCurve(ExpCurveName,"");
}
