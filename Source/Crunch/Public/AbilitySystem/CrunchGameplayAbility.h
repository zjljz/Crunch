// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include "CrunchGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCrunchGameplayAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr,
	                                const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereRadius = 30.f,
	                                                            ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = false, bool bIgnoreSelf = true) const;

	FGenericTeamId GetOwnerTeamId() const;
	bool IsOwnerTeamAttitudeTowards(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const;

	AActor* GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const;

	//给自己一个推力.
	void PushSelf(const FVector& PushVel) const;

	void PushTarget(AActor* Target, const FVector& PushVel) const;

	void PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel) const;
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& PushVel) const;

	//当我们不在ActivateAbility内部调用PlayMontageTask时 会出现这种情况: Ability所属的Client 无法播放Montage, 所以需要这个函数.
	void PlayMontageLocally(UAnimMontage* Montage) const;
	void StopMontageAfterCurrentSection(UAnimMontage* Montage) const;

	void ApplyGEToHitResultActor(const FHitResult& Hit, TSubclassOf<UGameplayEffect> EffectClass, int EffectLevel);

	//为了在本地更新准星,发送一个本地的GameplayEvent.
	void SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& Payload);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bShouldDrawDebug = false;
};
