// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CrunchGameplayAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FHitResult> UCrunchGameplayAbility::GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereRadius, bool bDrawDebug,
                                                                                    bool bIgnoreSelf) const
{
	TArray<FHitResult> OutRet;

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
		OutRet.Append(OutHits);
	}


	return OutRet;
}
