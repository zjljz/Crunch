// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RenderActor/SkeletalMeshRenderActor.h"

ASkeletalMeshRenderActor::ASkeletalMeshRenderActor()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	MeshComp->SetupAttachment(GetRootComponent());
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetLightingChannels(false, true, false);
}

void ASkeletalMeshRenderActor::BeginPlay()
{
	Super::BeginPlay();
	MeshComp->SetVisibleInSceneCaptureOnly(true);
}

void ASkeletalMeshRenderActor::ConfigureSkeletalMesh(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> AnimBP)
{
	MeshComp->SetSkeletalMeshAsset(NewMesh);
	MeshComp->SetAnimInstanceClass(AnimBP);
}
