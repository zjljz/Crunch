// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TargetActor_BlackHole.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"

ATargetActor_BlackHole::ATargetActor_BlackHole()
{
	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootComponent);

	DetectionSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphereComp"));
	DetectionSphereComp->SetupAttachment(RootComponent);
	DetectionSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnActorInBlackHoleRange);
	DetectionSphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnActorLeftBlackHoleRange);

	VFXComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("VFXComp"));
	VFXComp->SetupAttachment(RootComponent);
}

void ATargetActor_BlackHole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATargetActor_BlackHole, TeamId);

	DOREPLIFETIME_CONDITION_NOTIFY(ATargetActor_BlackHole, BlackHoleRange, COND_None, REPNOTIFY_Always);
}

void ATargetActor_BlackHole::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(BlackHoleDurationTimerHandle, this, &ThisClass::StopBlackHole, BlackHoleDuration);
	}
}

void ATargetActor_BlackHole::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		AActor* Target;
		UNiagaraComponent* Comp;

		for (auto& Pair : ActorsInRangeMap)
		{
			Target = Pair.Key;
			Comp = Pair.Value;

			FVector PullDir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
			Target->SetActorLocation(GetActorLocation() + PullDir * BlackHolePullSpeed * DeltaSeconds);

			if (Comp)
			{
				Comp->SetVariablePosition(BlackHoleVFXOriginName, VFXComp->GetComponentLocation());
			}
		}
	}
}

void ATargetActor_BlackHole::ConfirmTargetingAndContinue()
{
	StopBlackHole();
}

void ATargetActor_BlackHole::CancelTargeting()
{
	StopBlackHole();
	Super::CancelTargeting();
}

void ATargetActor_BlackHole::ConfigureBlackHole(float InRange, float InPullSpeed, float InDuration, const FGenericTeamId& InTeamId)
{
	BlackHoleRange = InRange;
	DetectionSphereComp->SetSphereRadius(BlackHoleRange);

	BlackHolePullSpeed = InPullSpeed;
	BlackHoleDuration = InDuration;

	SetGenericTeamId(InTeamId);
}

void ATargetActor_BlackHole::OnActorInBlackHoleRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                     const FHitResult& SweepResult)
{
	TryAddTarget(OtherActor);
}

void ATargetActor_BlackHole::OnActorLeftBlackHoleRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	RemoveTarget(OtherActor);
}

void ATargetActor_BlackHole::TryAddTarget(AActor* Target)
{
	if (!Target || ActorsInRangeMap.Contains(Target) || GetTeamAttitudeTowards(*Target) != ETeamAttitude::Hostile)
	{
		return;
	}

	UNiagaraComponent* LinkVFXComp = nullptr;
	if (BlackHoleLinkVFX)
	{
		LinkVFXComp = UNiagaraFunctionLibrary::SpawnSystemAttached(BlackHoleLinkVFX, Target->GetRootComponent(), NAME_None, FVector::Zero(), FRotator::ZeroRotator,
		                                                           EAttachLocation::Type::KeepRelativeOffset, false);
		if (LinkVFXComp)
		{
			LinkVFXComp->SetVariablePosition(BlackHoleVFXOriginName, VFXComp->GetComponentLocation());
		}
	}

	ActorsInRangeMap.Add(Target, LinkVFXComp);
}

void ATargetActor_BlackHole::RemoveTarget(AActor* Target)
{
	if (!Target) return;

	if (ActorsInRangeMap.Contains(Target))
	{
		UNiagaraComponent* Comp = nullptr;
		ActorsInRangeMap.RemoveAndCopyValue(Target, Comp);

		if (IsValid(Comp))
		{
			Comp->DestroyComponent();
		}
	}
}

void ATargetActor_BlackHole::StopBlackHole()
{
	TArray<TWeakObjectPtr<AActor>> FinalTargets;
	for (auto& Pair : ActorsInRangeMap)
	{
		if (Pair.Key)
		{
			FinalTargets.Add(Pair.Key);
			if (UNiagaraComponent* Comp = Pair.Value)
			{
				Comp->DestroyComponent();
			}
		}
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle;

	FGameplayAbilityTargetData_ActorArray* ArrayData = new FGameplayAbilityTargetData_ActorArray();
	ArrayData->SetActors(FinalTargets);
	TargetDataHandle.Add(ArrayData);

	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit();
	HitData->HitResult.ImpactPoint = GetActorLocation();
	TargetDataHandle.Add(HitData);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}

void ATargetActor_BlackHole::OnRep_BlackHoleRange()
{
	DetectionSphereComp->SetSphereRadius(BlackHoleRange);
}
