// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AN_SendTargetGroup.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Kismet/KismetSystemLibrary.h"

void UAN_SendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner() || TargetSocketNames.Num() <= 1 || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
	{
		return;
	}

	FGameplayEventData EventData;
	//TSet<AActor*> HitActors;
	AActor* OwnerActor = MeshComp->GetOwner();
	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwnerActor);

	for (int i = 1; i < TargetSocketNames.Num(); ++i)
	{
		/* 这里不再使用TargetData 是因为频繁使用这种方法来传递GameplayCue 会造成网络压力.
		FGameplayAbilityTargetData_LocationInfo* LocationInfo = new FGameplayAbilityTargetData_LocationInfo();
		LocationInfo->SourceLocation.LiteralTransform.SetLocation(StartLoc);
		LocationInfo->TargetLocation.LiteralTransform.SetLocation(EndLoc);
		EventData.TargetData.Add(LocationInfo);
		*/
		FVector StartLoc = MeshComp->GetSocketLocation(TargetSocketNames[i - 1]);
		FVector EndLoc = MeshComp->GetSocketLocation(TargetSocketNames[i]);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TArray<AActor*> IgnoreActors;
		if (bIgnoreOwner) IgnoreActors.Add(OwnerActor);
		TArray<FHitResult> OutResults;

		EDrawDebugTrace::Type DrawDebug = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		UKismetSystemLibrary::SphereTraceMultiForObjects(MeshComp, StartLoc, EndLoc, SphereSweepRadius, ObjectTypes, false,
		                                                 IgnoreActors, DrawDebug, OutResults, false);

		for (const FHitResult& Result : OutResults)
		{
			if (!Result.GetActor() /* ||  HitActors.Contains(Result.GetActor())*/) continue;
			
			if (OwnerTeamInterface)
			{
				if (OwnerTeamInterface->GetTeamAttitudeTowards(*Result.GetActor()) != TargetTeam)
				{
					continue;
				}
			}

			FGameplayAbilityTargetData_SingleTargetHit* TargetHitData = new FGameplayAbilityTargetData_SingleTargetHit(Result);
			EventData.TargetData.Add(TargetHitData);
			SendLocalGameplayCue(Result);
		}
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventData);
}

void UAN_SendTargetGroup::SendLocalGameplayCue(const FHitResult& HitResult) const
{
	FGameplayCueParameters CueParam;
	CueParam.Location = HitResult.Location;
	CueParam.Normal = HitResult.ImpactNormal;

	for (const FGameplayTag& Tag : TriggerGameplayCueTags)
	{
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(HitResult.GetActor(), Tag, EGameplayCueEvent::Executed, CueParam);
	}
}
