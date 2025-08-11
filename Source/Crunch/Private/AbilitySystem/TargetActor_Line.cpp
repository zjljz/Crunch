// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TargetActor_Line.h"
#include "NiagaraComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/SphereComponent.h"
#include "Crunch/Crunch.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ATargetActor_Line::ATargetActor_Line()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComponent);

	LazerVFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LazerVFXComp"));
	LazerVFXComp->SetupAttachment(RootComponent);

	TargetEndDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TargetEndDetectionSphere"));
	TargetEndDetectionSphere->SetupAttachment(RootComponent);
	TargetEndDetectionSphere->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore);

	AvatarActor = nullptr;
}

void ATargetActor_Line::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATargetActor_Line, OwnerTeamId);
	DOREPLIFETIME(ATargetActor_Line, TargetRange);
	DOREPLIFETIME(ATargetActor_Line, DetectionCylinderRadius);
	DOREPLIFETIME(ATargetActor_Line, AvatarActor);
}

void ATargetActor_Line::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateTargetTrack();
}

void ATargetActor_Line::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	if (!OwningAbility) return;

	AvatarActor = OwningAbility->GetAvatarActorFromActorInfo();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(PeriodicalTargetingTimerHandle, this, &ThisClass::DoTargetCheckAndReport, TargetingInterval, true);
	}
}

void ATargetActor_Line::ConfigureTargetSetting(float NewTargetRange, float NewDetectionCylinderRadius, float NewTargetingInterval, FGenericTeamId NewTeamId, bool NewbDrawDebug)
{
	TargetRange = NewTargetRange;
	DetectionCylinderRadius = NewDetectionCylinderRadius;
	TargetingInterval = NewTargetingInterval;
	SetGenericTeamId(NewTeamId);
	bShouldDrawDebug = NewbDrawDebug;
}

void ATargetActor_Line::DoTargetCheckAndReport()
{
	if (!HasAuthority()) return;

	TSet<AActor*> OverlappedActorSet;
	TargetEndDetectionSphere->GetOverlappingActors(OverlappedActorSet);

	TArray<TWeakObjectPtr<AActor>> OverlappedActorArr;
	
	for (AActor* Actor : OverlappedActorSet)
	{
		if (ShouldReportActorAsTarget(Actor))
		{
			OverlappedActorArr.AddUnique(Actor);
		}
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_ActorArray* ActorArrayData = new FGameplayAbilityTargetData_ActorArray();
	ActorArrayData->SetActors(OverlappedActorArr);
	TargetDataHandle.Add(ActorArrayData);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

void ATargetActor_Line::UpdateTargetTrack()
{
	FVector ViewLoc = GetActorLocation();
	FRotator ViewRot = GetActorRotation();
	if (AvatarActor)
	{
		AvatarActor->GetActorEyesViewPoint(ViewLoc, ViewRot);
	}

	FVector LookEndPoint = ViewLoc + ViewRot.Vector() * 100000.f;
	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookEndPoint);
	SetActorRotation(LookRot);

	FVector SweepEndLoc = GetActorLocation() + LookRot.Vector() * TargetRange;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Param;
	Param.AddIgnoredActor(AvatarActor);
	Param.AddIgnoredActor(this);
	FCollisionResponseParams CRParam(ECR_Overlap);

	GetWorld()->SweepMultiByChannel(HitResults, GetActorLocation(), SweepEndLoc, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(DetectionCylinderRadius), Param, CRParam);
	FVector LineEndLoc = SweepEndLoc;
	float LineLength = TargetRange;

	for (FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor())
		{
			if (GetTeamAttitudeTowards(*Hit.GetActor()) != ETeamAttitude::Friendly)
			{
				LineEndLoc = Hit.ImpactPoint;
				LineLength = FVector::Distance(GetActorLocation(), LineEndLoc);
				break;
			}
		}
	}

	TargetEndDetectionSphere->SetWorldLocation(LineEndLoc);
	if (LazerVFXComp)
	{
		LazerVFXComp->SetVariableFloat(LazerFXLengthParamName, LineLength / 100.f);
	}
}

bool ATargetActor_Line::ShouldReportActorAsTarget(const AActor* InActor) const
{
	if (!InActor || InActor == AvatarActor || InActor == this) return false;

	if (GetTeamAttitudeTowards(*InActor) != ETeamAttitude::Hostile) return false;

	return true;
}
