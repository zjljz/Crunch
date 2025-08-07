// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/AbilitySystem/Ability/Ability_DeadExp.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/CrunchAbilitySystemStatics.h"
#include "AbilitySystem/CrunchHeroAttributeSet.h"
#include "Crunch/CrunchGameplayTags.h"
#include "Engine/OverlapResult.h"

UAbility_DeadExp::UAbility_DeadExp()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = CrunchGameplayTags::Stats_Dead;
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.RemoveTag(CrunchGameplayTags::Stats_Stun);
}

void UAbility_DeadExp::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (K2_HasAuthority())
	{
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
		if (!Killer || !UCrunchAbilitySystemStatics::IsActorHaveTag(Killer, CrunchGameplayTags::Role_Hero))
		{
			//@todo: 这里可以添加一个Log 例如: xx被xx Kill.
			Killer = nullptr;
		}

		TArray<AActor*> RewardActors = GetRewardTargets();
		if (RewardActors.Num() == 0 || !RewardEffectClass)
		{
			K2_EndAbility();
			return;
		}

		if (Killer && !RewardActors.Contains(Killer))
		{
			RewardActors.Add(Killer);
		}

		bool bFound = false;
		float SelfExp = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(UCrunchHeroAttributeSet::GetExperienceAttribute(), bFound);

		float TotalExpReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExp;
		float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfExp;

		if (Killer)
		{
			float KillerExpReward = TotalExpReward * KillerRewardPercent;
			float KillerGoldReward = TotalGoldReward * KillerRewardPercent;

			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffectClass);
			SpecHandle.Data->SetSetByCallerMagnitude(CrunchGameplayTags::Attr_Experience, KillerExpReward);
			SpecHandle.Data->SetSetByCallerMagnitude(CrunchGameplayTags::Attr_Gold, KillerGoldReward);

			K2_ApplyGameplayEffectSpecToTarget(SpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));

			TotalExpReward -= KillerExpReward;
			TotalGoldReward -= KillerGoldReward;
		}

		float ExpPerTarget = TotalExpReward / RewardActors.Num();
		float GoldPerTarget = TotalGoldReward / RewardActors.Num();

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffectClass);
		SpecHandle.Data->SetSetByCallerMagnitude(CrunchGameplayTags::Attr_Experience, ExpPerTarget);
		SpecHandle.Data->SetSetByCallerMagnitude(CrunchGameplayTags::Attr_Gold, GoldPerTarget);

		K2_ApplyGameplayEffectSpecToTarget(SpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardActors, true));
		K2_EndAbility();
	}
}

TArray<AActor*> UAbility_DeadExp::GetRewardTargets() const
{
	TSet<AActor*> OutActors;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld()) return OutActors.Array();

	TArray<FOverlapResult> OverlapRes;
	FCollisionObjectQueryParams Params;
	Params.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape Shape;
	Shape.SetSphere(RewardRange);

	if (GetWorld()->OverlapMultiByObjectType(OverlapRes, AvatarActor->GetActorLocation(), FQuat::Identity, Params, Shape))
	{
		for (auto Result : OverlapRes)
		{
			IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(Result.GetActor());
			if (OtherTeamInterface && OtherTeamInterface->GetTeamAttitudeTowards(*AvatarActor) == ETeamAttitude::Hostile)
			{
				if (UCrunchAbilitySystemStatics::IsActorHaveTag(Result.GetActor(), CrunchGameplayTags::Role_Hero))
				{
					OutActors.Add(Result.GetActor());
				}
			}
		}
	}

	return OutActors.Array();
}
