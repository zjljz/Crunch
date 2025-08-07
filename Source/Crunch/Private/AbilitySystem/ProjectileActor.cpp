// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ProjectileActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Net/UnrealNetwork.h"


AProjectileActor::AProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComponent);
}

void AProjectileActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectileActor, MoveDirection);
	DOREPLIFETIME(AProjectileActor, TeamId);
	DOREPLIFETIME(AProjectileActor, ProjectileSpeed);
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (TargetActor)
		{
			MoveDirection = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		}
	}

	SetActorLocation(GetActorLocation() + MoveDirection * ProjectileSpeed * DeltaTime);
}

void AProjectileActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile)
	{
		return;
	}

	UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (IsValid(OtherASC))
	{
		if (HasAuthority() && HitEffectSpecHandle.IsValid())
		{
			OtherASC->ApplyGameplayEffectSpecToSelf(*HitEffectSpecHandle.Data.Get());
			GetWorldTimerManager().ClearTimer(ShootTimeHandle);
		}

		FHitResult HitResult;
		HitResult.Location = GetActorLocation();
		HitResult.ImpactNormal = GetActorForwardVector();
		SendLocalGameplayCue(OtherActor, HitResult);
		
		Destroy();
	}
}

void AProjectileActor::ShootProjectile(float InSpeed, float InMaxDistance, const AActor* InTarget, FGenericTeamId InTeamId, FGameplayEffectSpecHandle InHitEffectHandle)
{
	ProjectileSpeed = InSpeed;
	TargetActor = InTarget;
	SetGenericTeamId(InTeamId);
	HitEffectSpecHandle = InHitEffectHandle;

	FRotator OwnerViewRot = GetActorRotation();
	if (GetOwner())
	{
		FVector OwnerViewLoc;
		GetOwner()->GetActorEyesViewPoint(OwnerViewLoc, OwnerViewRot);
	}

	MoveDirection = OwnerViewRot.Vector();

	float TravelMaxTime = InMaxDistance / InSpeed;
	GetWorldTimerManager().SetTimer(ShootTimeHandle, this, &ThisClass::TravelMaxDistanceReached, TravelMaxTime);
}

void AProjectileActor::TravelMaxDistanceReached()
{
	Destroy();
}

void AProjectileActor::SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult) const
{
	FGameplayCueParameters CueParams;
	CueParams.Location = HitResult.Location;
	CueParams.Normal = HitResult.ImpactNormal;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(CueTargetActor, HitCueTag, EGameplayCueEvent::Executed, CueParams);
}
