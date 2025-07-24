// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CrunchGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Crunch/CrunchGameplayTags.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UCrunchGameplayAbility::UCrunchGameplayAbility()
{
	ActivationBlockedTags.AddTag(CrunchGameplayTags::Stats_Stun);
}

TArray<FHitResult> UCrunchGameplayAbility::GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereRadius,
                                                                                    ETeamAttitude::Type TargetTeam, bool bDrawDebug, bool bIgnoreSelf) const
{
	TArray<FHitResult> OutRet;
	TSet<AActor*> HitActors;

	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

	for (const auto Data : TargetDataHandle.Data)
	{
		//虽然我们在AN_SendTargetGroup中 存入的是FGameplayAbilityTargetData_LocationInfo, 但是子类重写了父类函数 可以得到存入的结果.
		FVector StartLoc = Data->GetOrigin().GetTranslation();
		FVector EndLoc = Data->GetEndPoint();

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> IgnoreActors;
		if (bIgnoreSelf)
		{
			IgnoreActors.Add(GetAvatarActorFromActorInfo());
		}

		EDrawDebugTrace::Type DrawType = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		TArray<FHitResult> OutHits;
		UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLoc, EndLoc, SphereRadius,
		                                                 ObjectTypes, false, IgnoreActors, DrawType, OutHits, false);

		for (const FHitResult& Hit : OutHits)
		{
			if (HitActors.Contains(Hit.GetActor()))
			{
				continue; //如果已经命中过这个Actor, 则跳过.
			}

			if (OwnerTeamInterface)
			{
				ETeamAttitude::Type OtherActorTeam = OwnerTeamInterface->GetTeamAttitudeTowards(*Hit.GetActor());
				if (OtherActorTeam != TargetTeam)
				{
					continue;
				}
			}

			HitActors.Add(Hit.GetActor());
			OutRet.Add(Hit);
		}
	}

	return OutRet;
}

void UCrunchGameplayAbility::PushSelf(const FVector& PushVel)
{
	if (ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		AvatarCharacter->LaunchCharacter(PushVel, true, true);
	}
}

void UCrunchGameplayAbility::PushTarget(AActor* Target, const FVector& PushVel)
{
	if (!Target)
	{
		return;
	}

	FGameplayEventData EventData;
	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit();
	FHitResult HitResult;
	HitResult.ImpactNormal = PushVel;
	HitData->HitResult = HitResult;
	EventData.TargetData.Add(HitData);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, CrunchGameplayTags::Ability_Passive_Launch_Activate, EventData);
}

void UCrunchGameplayAbility::PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel)
{
	for (auto Target : Targets)
	{
		PushTarget(Target, PushVel);
	}
}

void UCrunchGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& PushVel)
{
	TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetData);
	PushTargets(TargetActors, PushVel);
}

void UCrunchGameplayAbility::ApplyGEToHitResultActor(const FHitResult& Hit, TSubclassOf<UGameplayEffect> EffectClass, int EffectLevel)
{
	FGameplayEffectSpecHandle EffectSpectHandle = MakeOutgoingGameplayEffectSpec(EffectClass, EffectLevel);
	FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
	EffectContextHandle.AddHitResult(Hit);
	EffectSpectHandle.Data->SetContext(EffectContextHandle);
		
	ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpectHandle,
									UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Hit.GetActor()));
}
