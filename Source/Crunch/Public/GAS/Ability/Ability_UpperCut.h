// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CrunchGameplayAbility.h"
#include "GAS/CrunchGameplayAbilityTypes.h"
#include "Ability_UpperCut.generated.h"

/**
 *  一个上勾拳的Gameplay Ability.
 */
UCLASS()
class CRUNCH_API UAbility_UpperCut : public UCrunchGameplayAbility
{
	GENERATED_BODY()

public:

	UAbility_UpperCut();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void StartLaunching(FGameplayEventData Payload);

	UFUNCTION()
	void HandleComboChangeEvent(FGameplayEventData Payload);
	
	UFUNCTION()
	void HandleComboCommitEvent(FGameplayEventData Payload);
	
	UFUNCTION()
	void HandleComboDamageEvent(FGameplayEventData Payload);

	// 获取当前Combo名称对应的伤害GE.
	const FGenericDamageEffectDef* GetComboDamageEffectDef() const;

	
private:
	// 上勾拳的伤害GE映射, 用于不同Combo名称对应不同的伤害GE,同时定义GE的推力.
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TMap<FName, FGenericDamageEffectDef> ComboDamageEffectMap;
	
	// 上勾拳的伤害GE.
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;

	// 上勾拳的动画Montage.
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> UpperCutMontage;

	// 目标范围的半径, 用于检测命中的敌人.
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetSweepSphereRadius = 80.f;

	// 向上推力的速度.
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float UpperCutLaunchSpeed = 1000.f;

	FName NextComboSectionName = NAME_None; // 下一个Combo的Section名称, 用于Montage的播放.
};
