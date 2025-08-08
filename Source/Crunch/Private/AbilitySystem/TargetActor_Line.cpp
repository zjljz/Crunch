// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TargetActor_Line.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Crunch/Crunch.h"

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
