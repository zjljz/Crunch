// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/AbilitySystem/Ability/Ability_GroundBlast.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Crunch/CrunchGameplayTags.h"
#include "Crunch/Public/AbilitySystem/TargetActor_GroundPick.h"

UAbility_GroundBlast::UAbility_GroundBlast()
{
	ActivationOwnedTags.AddTag(CrunchGameplayTags::Stats_Aim);
	BlockAbilitiesWithTag.AddTag(CrunchGameplayTags::Ability_BasicAttack);
}

void UAbility_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}

	
	UAbilityTask_PlayMontageAndWait* GroundBlastMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	GroundBlastMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	GroundBlastMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	GroundBlastMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	GroundBlastMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	GroundBlastMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData
		(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);

	WaitTargetDataTask->ValidData.AddDynamic(this, &ThisClass::OnTargetConfirmed);
	WaitTargetDataTask->Cancelled.AddDynamic(this, &ThisClass::OnTargetCancelled);
	WaitTargetDataTask->ReadyForActivation();

	// 这里我们只在OwningClient上生成TA, 一般来说AI的GA才会使用ShouldProduceTargetDataOnServer 此时会在Server上生成TA.
	AGameplayAbilityTargetActor* TargetActor = nullptr;
	WaitTargetDataTask->BeginSpawningActor(this, TargetActorClass, TargetActor);
	if (ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor))
	{
		GroundPickActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickActor->SetDrawDebug(ShouldDrawDebug());
		GroundPickActor->SetTraceLineLength(TraceLineLength);
	}

	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);
}

void UAbility_GroundBlast::OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& Data)
{
	if (!K2_CommitAbility())
	{
		//如果没有成功Commit这个Ability，则结束这个Ability
		UE_LOG(LogTemp, Error, TEXT("CommitAbility fault on GroundBlast ! ! !"))
		K2_EndAbility();
		return;
	}
	
	if (HasAuthority(&CurrentActivationInfo))
	{
		BP_ApplyGameplayEffectToTarget(Data, DamageEffectDef.DamageEffect, GetAbilityLevel());
		PushTargets(Data, DamageEffectDef.PushVelocity);
	}


	FGameplayCueParameters CueParams;
	CueParams.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data, 1).ImpactPoint;
	CueParams.RawMagnitude = TargetAreaRadius;
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(BlasterCueTag, CueParams);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(CrunchGameplayTags::GameplayCue_CameraShake);

	if (UAnimInstance* AnimInst = GetActorInfo().SkeletalMeshComponent->GetAnimInstance())
	{
		AnimInst->Montage_Play(CastMontage);
	}

	K2_EndAbility();
}

void UAbility_GroundBlast::OnTargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Target Cancelled!"));
	K2_EndAbility();
}
