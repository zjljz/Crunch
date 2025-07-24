// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor_GroundPick.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Components/DecalComponent.h"
#include "Crunch/Crunch.h"
#include "Engine/OverlapResult.h"

ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootScene"));
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetupAttachment(RootComponent);
}

void ATargetActor_GroundPick::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
	{
		SetActorLocation(GetTargetPoint());
	}
}

FVector ATargetActor_GroundPick::GetTargetPoint() const
{
	if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
	{
		FHitResult Hit;
		FVector VieLoc;
		FRotator VieRot;
		PrimaryPC->GetPlayerViewPoint(VieLoc, VieRot);
		FVector EndLoc = VieLoc + VieRot.Vector() * TraceLineLength; // Adjust the distance as needed

		GetWorld()->LineTraceSingleByChannel(Hit, VieLoc, EndLoc, ECC_Target);

		if (!Hit.bBlockingHit)
		{
			GetWorld()->LineTraceSingleByChannel(Hit, EndLoc, EndLoc + FVector::DownVector * TNumericLimits<float>::Max(), ECC_Target);
		}

		if (!Hit.bBlockingHit)
		{
			return GetActorLocation();
		}

		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TargetAreaRadius, 32, FColor::Red);
		}

		return Hit.ImpactPoint;
	}

	return GetActorLocation();
}

void ATargetActor_GroundPick::ConfirmTargetingAndContinue()
{
	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByObjectType(OverlapResults, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn),
	                                     FCollisionShape::MakeSphere(TargetAreaRadius));

	TSet<AActor*> TargetActors;

	IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwningAbility->GetAvatarActorFromActorInfo());
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*Result.GetActor()) == ETeamAttitude::Friendly && !bShouldTargetFriendly)
		{
			continue;
		}

		if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*Result.GetActor()) == ETeamAttitude::Hostile && !bShouldTargetEnemy)
		{
			continue;
		}

		TargetActors.Add(Result.GetActor());
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(TargetActors.Array(), false);
	FGameplayAbilityTargetData_SingleTargetHit* HitLoc = new FGameplayAbilityTargetData_SingleTargetHit();
	HitLoc->HitResult.ImpactPoint = GetActorLocation();
	TargetDataHandle.Add(HitLoc);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

void ATargetActor_GroundPick::SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy)
{
	bShouldTargetFriendly = bTargetFriendly;
	bShouldTargetEnemy = bTargetEnemy;
}

void ATargetActor_GroundPick::SetTargetAreaRadius(float NewRadius)
{
	TargetAreaRadius = NewRadius;
	DecalComp->DecalSize = FVector(NewRadius);
}
