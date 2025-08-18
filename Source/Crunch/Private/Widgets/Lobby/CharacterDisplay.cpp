// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/CharacterDisplay.h"

#include "Camera/CameraComponent.h"
#include "Character/PawnData.h"

ACharacterDisplay::ACharacterDisplay()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComponent);
	
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	MeshComp->SetupAttachment(GetRootComponent());
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(GetRootComponent());
}

void ACharacterDisplay::ConfigureFromPawnData(const UPawnData* InPawnData)
{
	if (!InPawnData) return;

	MeshComp->SetSkeletalMesh(InPawnData->GetCharacterMesh());
	MeshComp->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
	MeshComp->SetAnimInstanceClass(InPawnData->GetOrLoadCharacterAnimInstance());
}
