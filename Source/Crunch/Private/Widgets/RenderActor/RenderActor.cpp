// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RenderActor/RenderActor.h"

#include "Components/SceneCaptureComponent2D.h"

ARenderActor::ARenderActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComponent);

	CaptureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComp"));
	CaptureComp->SetupAttachment(RootComponent);

	CaptureComp->bCaptureEveryFrame = false;
	CaptureComp->FOVAngle = 30.f;
}

void ARenderActor::BeginPlay()
{
	Super::BeginPlay();

	CaptureComp->ShowOnlyActorComponents(this);
	SetActorLocation(FVector{0.f, 0.f, 100000.f});
}

void ARenderActor::SetRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
	CaptureComp->TextureTarget = RenderTarget;
}

void ARenderActor::UpdateRender()
{
	if (CaptureComp)
	{
		CaptureComp->CaptureScene();
	}
}
