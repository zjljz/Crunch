// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CrunchAnimInstanceBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Crunch/CrunchGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCrunchAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}

	UAbilitySystemComponent* OwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TryGetPawnOwner());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(CrunchGameplayTags::Stats_Aim, EGameplayTagEventType::NewOrRemoved)
			.AddLambda([this](FGameplayTag Tag, int32 NewCount)
			{
				bIsAiming = (NewCount != 0);
			});
	}
}

void UCrunchAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerCharacter)
	{
		FVector Vel = OwnerMovementComp->Velocity;
		Speed = OwnerMovementComp ? Vel.Length() : 0.f;
		FRotator BodyRot = OwnerCharacter->GetActorRotation();
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(BodyRot,BodyLastRot);
		BodyLastRot = BodyRot;

		YawSpeed = DeltaRot.Yaw / DeltaSeconds;
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, SmoothLerpSpeedForYawSpeed);

		FRotator ControlRot = OwnerCharacter->GetBaseAimRotation();
		LookRotOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, BodyRot);

		FwdSpeed = Vel.Dot(ControlRot.Vector());
		RightSpeed = -Vel.Dot(ControlRot.Vector().Cross(FVector::UpVector));
	}

	if (OwnerMovementComp)
	{
		bIsJumping = OwnerMovementComp->IsFalling();
	}
}

void UCrunchAnimInstanceBase::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

bool UCrunchAnimInstanceBase::bShouldDoFullBody() const
{
	return (GetSpeed() <= 0.0f) && !GetIsAiming();
}
