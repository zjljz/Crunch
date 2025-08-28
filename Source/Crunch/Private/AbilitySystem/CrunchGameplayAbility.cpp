// Fill out your copyright notice in the Description page of Project Settings.


#include "Crunch/Public/AbilitySystem/CrunchGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Crunch/CrunchGameplayTags.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

UCrunchGameplayAbility::UCrunchGameplayAbility()
{
	//@todo: 为所有能力添加了一个Stun的BlockedTag, 也就是说在Stun时 会Block其他Ability的Activate.
	ActivationBlockedTags.AddTag(CrunchGameplayTags::Stats_Stun);
}

bool UCrunchGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle((Handle));
	if (AbilitySpec && AbilitySpec->Level <= 0)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

TArray<FHitResult> UCrunchGameplayAbility::GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereRadius,
                                                                                    ETeamAttitude::Type TargetTeam, bool bDrawDebug, bool bIgnoreSelf) const
{
	TArray<FHitResult> OutRet;
	TSet<AActor*> HitActors;

	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

	for (const auto& Data : TargetDataHandle.Data)
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

FGenericTeamId UCrunchGameplayAbility::GetOwnerTeamId() const
{
	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerTeamInterface)
	{
		return OwnerTeamInterface->GetGenericTeamId();
	}

	return FGenericTeamId::NoTeam;
}

bool UCrunchGameplayAbility::IsOwnerTeamAttitudeTowards(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const
{
	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	if (OwnerTeamInterface && OtherActor)
	{
		return OwnerTeamInterface->GetTeamAttitudeTowards(*OtherActor) == TeamAttitude;
	}

	return false;
}

AActor* UCrunchGameplayAbility::GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const
{
	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		FVector Loc;
		FRotator Rot;

		AvatarActor->GetActorEyesViewPoint(Loc, Rot);
		FVector AimEnd = Loc + Rot.Vector() * AimDistance;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(AvatarActor);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		if (bShouldDrawDebug)
		{
			DrawDebugLine(GetWorld(), Loc, AimEnd, FColor::Red, false, 2.f, 0, 3.f);
		}

		TArray<FHitResult> HitResults;
		if (GetWorld()->LineTraceMultiByObjectType(HitResults, Loc, AimEnd, ObjectQueryParams, Params))
		{
			for (FHitResult& Hit : HitResults)
			{
				if (IsOwnerTeamAttitudeTowards(Hit.GetActor(), TeamAttitude))
				{
					return Hit.GetActor();
				}
			}
		}
	}

	return nullptr;
}

void UCrunchGameplayAbility::PushSelf(const FVector& PushVel) const
{
	if (ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		AvatarCharacter->LaunchCharacter(PushVel, true, true);
	}
}

void UCrunchGameplayAbility::PushTarget(AActor* Target, const FVector& PushVel) const
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

void UCrunchGameplayAbility::PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel) const
{
	for (auto Target : Targets)
	{
		PushTarget(Target, PushVel);
	}
}

void UCrunchGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& PushVel) const
{
	TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetData);
	PushTargets(TargetActors, PushVel);
}

void UCrunchGameplayAbility::PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation, float PushSpeed) const
{
	for (AActor* Target : Targets)
	{
		FVector PushDir = Target->GetActorLocation() - FromLocation;
		PushDir.Z = 0.f;
		PushDir.Normalize();
		PushTarget(Target, PushDir * PushSpeed);
	}
}

void UCrunchGameplayAbility::PushTargetsFromLocation(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& FromLocation, float PushSpeed) const
{
	TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetData);
	PushTargetsFromLocation(Targets, FromLocation, PushSpeed);
}

void UCrunchGameplayAbility::PlayMontageLocally(UAnimMontage* Montage) const
{
	UAnimInstance* OwnerAnimInst = CurrentActorInfo->GetAnimInstance();
	if (OwnerAnimInst && !OwnerAnimInst->Montage_IsPlaying(Montage))
	{
		OwnerAnimInst->Montage_Play(Montage);
	}
}

void UCrunchGameplayAbility::StopMontageAfterCurrentSection(UAnimMontage* Montage) const
{
	if (UAnimInstance* OwnerAnimInst = CurrentActorInfo->GetAnimInstance())
	{
		FName CurSectionName = OwnerAnimInst->Montage_GetCurrentSection(Montage);
		OwnerAnimInst->Montage_SetNextSection(CurSectionName, NAME_None, Montage);
	}
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

void UCrunchGameplayAbility::SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& Payload)
{
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		OwnerASC->HandleGameplayEvent(EventTag, &Payload);
	}
}
