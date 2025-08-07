// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

struct FGameplayEventData;
class UCanvasPanelSlot;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void OnCrosshairTagUpdate(const FGameplayTag Tag, int32 NewCount);

	void UpdateCrosshairPosition();

	void OnAimTargetUpdated(const FGameplayEventData* Data);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Crosshair;

	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CrosshairPanelSlot;

	UPROPERTY()
	TObjectPtr<APlayerController> OwnerPC;

	UPROPERTY()
	TObjectPtr<const AActor> AimTarget;

	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor HaveTargetColor = FLinearColor::Red;
	
	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor NoTargetColor = FLinearColor::White;
};
