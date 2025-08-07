// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RenderActor.generated.h"

UCLASS()
class CRUNCH_API ARenderActor : public AActor
{
	GENERATED_BODY()

public:
	ARenderActor();

protected:
	virtual void BeginPlay() override;

public:
	void SetRenderTarget(UTextureRenderTarget2D* RenderTarget);
	void UpdateRender();

	FORCEINLINE USceneCaptureComponent2D* GetCaptureComponent() const { return CaptureComp; }

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Render Actor")
	TObjectPtr<USceneCaptureComponent2D> CaptureComp;
};
