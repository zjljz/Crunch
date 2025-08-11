// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/StormCore.h"

#include "AIController.h"
#include "Components/SphereComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AStormCore::AStormCore()
{
	PrimaryActorTick.bCanEverTick = true;

	InfluenceSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("InfluenceSphereComp"));
	InfluenceSphereComp->SetupAttachment(GetRootComponent());

	InfluenceSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::NewInfluenceInRange);
	InfluenceSphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::InfluenceLeftRange);

	ViewCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCameraComp"));
	ViewCameraComp->SetupAttachment(GetRootComponent());

	GroundDecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("GroundDecalComp"));
	GroundDecalComp->SetupAttachment(GetRootComponent());
}

void AStormCore::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AStormCore, CoreToCapture, COND_None, REPNOTIFY_Always);
}

void AStormCore::BeginPlay()
{
	Super::BeginPlay();

	FVector TeamOneGoalLoc = TeamOneGoal->GetActorLocation();
	FVector TeamTwoGoalLoc = TeamTwoGoal->GetActorLocation();

	FVector GoalOffset = TeamOneGoalLoc - TeamTwoGoalLoc;
	GoalOffset.Z = 0.f;

	TravelLength = GoalOffset.Length();
}

void AStormCore::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	OwnerAIC = Cast<AAIController>(NewController);
}

void AStormCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CoreToCapture)
	{
		FVector CoreMoveDir = (GetMesh()->GetComponentLocation() - CoreToCapture->GetActorLocation()).GetSafeNormal();
		CoreToCapture->AddActorWorldOffset(CoreMoveDir * CoreCaptureSpeed * DeltaTime);
	}
}


void AStormCore::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AStormCore, InfluenceRadius))
	{
		InfluenceSphereComp->SetSphereRadius(InfluenceRadius);
		FVector DecalSize = GroundDecalComp->DecalSize;
		GroundDecalComp->DecalSize = FVector{DecalSize.X, InfluenceRadius, InfluenceRadius};
	}
}

float AStormCore::GetProgress() const
{
	FVector TeamTwoGoalLoc = TeamTwoGoal->GetActorLocation();
	FVector VectorFromTeamOne = GetActorLocation() - TeamTwoGoalLoc;
	VectorFromTeamOne.Z = 0.f;

	return VectorFromTeamOne.Length() / TravelLength;
}

void AStormCore::NewInfluenceInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == TeamOneGoal)
	{
		OnGoalReached(0);
	}
	else if (OtherActor == TeamTwoGoal)
	{
		OnGoalReached(1);
	}

	if (IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(OtherActor))
	{
		if (TeamAgentInterface->GetGenericTeamId().GetId() == 0)
		{
			TeamOneInfluenceCount++;
		}
		else if (TeamAgentInterface->GetGenericTeamId().GetId() == 1)
		{
			TeamTwoInfluenceCount++;
		}

		UpdateTeamWeight();
	}
}

void AStormCore::InfluenceLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(OtherActor))
	{
		if (TeamAgentInterface->GetGenericTeamId().GetId() == 0)
		{
			TeamOneInfluenceCount--;

			if (TeamOneInfluenceCount < 0) TeamOneInfluenceCount = 0;
		}
		else if (TeamAgentInterface->GetGenericTeamId().GetId() == 1)
		{
			TeamTwoInfluenceCount--;

			if (TeamTwoInfluenceCount < 0) TeamTwoInfluenceCount = 0;
		}

		UpdateTeamWeight();
	}
}

void AStormCore::UpdateTeamWeight()
{
	OnTeamInfluenceCountUpdateDelegate.Broadcast(TeamOneInfluenceCount, TeamTwoInfluenceCount);

	if (TeamOneInfluenceCount == TeamTwoInfluenceCount)
	{
		TeamWeight = 0.f;
	}
	else
	{
		float TeamOffset = TeamOneInfluenceCount - TeamTwoInfluenceCount;
		float TeamTotal = TeamOneInfluenceCount + TeamTwoInfluenceCount;
		TeamWeight = TeamOffset / TeamTotal;
	}

	FString DebugStr = FString::Printf(TEXT("Team One Count : %d, Team Two Count : %d, Weight : %f"), TeamOneInfluenceCount, TeamTwoInfluenceCount, TeamWeight);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, DebugStr);

	UpdateGoal();
}

void AStormCore::UpdateGoal()
{
	if (!HasAuthority() || !OwnerAIC || !GetCharacterMovement()) return;

	if (TeamWeight > 0.f)
	{
		OwnerAIC->MoveToActor(TeamOneGoal);
	}
	else
	{
		OwnerAIC->MoveToActor(TeamTwoGoal);
	}

	float Speed = MaxMoveSpeed * FMath::Abs(TeamWeight);
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AStormCore::OnGoalReached(int WinTeamId)
{
	OnGoalReachedDelegate.Broadcast(this, WinTeamId);

	if (!HasAuthority()) return;

	MaxMoveSpeed = 0.f;

	CoreToCapture = WinTeamId == 0 ? TeamTwoCore : TeamOneCore;
	CaptureCore();
}

void AStormCore::CaptureCore()
{
	float ExpandDuration = GetMesh()->GetAnimInstance()->Montage_Play(ExpandMontage);
	CoreCaptureSpeed = FVector::Distance(GetMesh()->GetComponentLocation(), CoreToCapture->GetActorLocation()) / ExpandDuration;

	CoreToCapture->SetActorEnableCollision(false);
	GetCharacterMovement()->MaxWalkSpeed = 0.f;

	FTimerHandle ExpandTimerHandle;
	GetWorldTimerManager().SetTimer(ExpandTimerHandle, this, &ThisClass::ExpandFinished, ExpandDuration);
}

void AStormCore::ExpandFinished()
{
	CoreToCapture->SetActorLocation(GetMesh()->GetComponentLocation());
	CoreToCapture->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "Root");
	GetMesh()->GetAnimInstance()->Montage_Play(CaptureMontage);
}

void AStormCore::OnRep_CoreToCapture()
{
	if (CoreToCapture)
	{
		CaptureCore();
	}
}
