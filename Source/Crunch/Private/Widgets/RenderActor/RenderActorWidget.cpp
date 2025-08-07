// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RenderActor/RenderActorWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Widgets/RenderActor/RenderActor.h"

void URenderActorWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	RenderSizeBox->SetWidthOverride(RenderSize.X);
	RenderSizeBox->SetHeightOverride(RenderSize.Y);
}

void URenderActorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SpawnRenderActor();
	ConfigureRenderActor();
	BeginRenderCapture();
}

void URenderActorWidget::BeginDestroy()
{
	StopRenderCapture();

	Super::BeginDestroy();
}

void URenderActorWidget::ConfigureRenderActor()
{
	if (!GetRenderActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Render Actor, Nothing will be Captured."));
		return;
	}

	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(RenderSize.X, RenderSize.Y);
	RenderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
	GetRenderActor()->SetRenderTarget(RenderTarget);

	if (UMaterialInstanceDynamic* DynamicMaterialIns = Img_Display->GetDynamicMaterial())
	{
		DynamicMaterialIns->SetTextureParameterValue(DisplayImageRenderTargetParamName, RenderTarget);
	}
}

void URenderActorWidget::BeginRenderCapture()
{
	RenderTickInterval = 1.f / (float)FrameRate;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RenderTimerHandle, this, &ThisClass::UpdateRenderCapture, RenderTickInterval, true);
	}
}

void URenderActorWidget::UpdateRenderCapture()
{
	if (GetRenderActor())
	{
		GetRenderActor()->UpdateRender();
	}
}

void URenderActorWidget::StopRenderCapture()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RenderTimerHandle);
	}
}
