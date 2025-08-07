// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RenderActorWidget.generated.h"

class ARenderActor;
class USizeBox;
class UImage;

/**
 * 
 */
UCLASS(Abstract)
class CRUNCH_API URenderActorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	virtual void BeginDestroy() override;

private:
	virtual void SpawnRenderActor() PURE_VIRTUAL(URenderActorWidget::SpawnRenderActor,);
	virtual ARenderActor* GetRenderActor() const PURE_VIRTUAL(URenderActorWidget::SpawnRenderActor, return nullptr;);

	void ConfigureRenderActor();

	void BeginRenderCapture();
	void UpdateRenderCapture();
	void StopRenderCapture();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Display;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> RenderSizeBox;

	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	FName DisplayImageRenderTargetParamName = "RenderTarget";

	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	FVector2D RenderSize;

	UPROPERTY(EditDefaultsOnly, Category = "Render Actor")
	int FrameRate = 24;

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	float RenderTickInterval;
	FTimerHandle RenderTimerHandle;
};
