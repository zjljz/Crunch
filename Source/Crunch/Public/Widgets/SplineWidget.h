// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SplineWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API USplineWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupSpline(const UUserWidget* InStartWidget, const UUserWidget* InEndWidget,
	                 FVector2D& InStartPortLocalCoord, FVector2D& InEndPortLocalCoord,
	                 FVector2D& InStartPortDirection, FVector2D& InEndPortDirection);

	void SetSplineStyle(const FLinearColor& InColor, float InThickness);

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	                          const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D TestStartPos;

	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D TestEndPos = FVector2D{100, 100};

	UPROPERTY(EditAnywhere, Category = "Spline")
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Spline")
	float Thickness = 3.f;

	UPROPERTY()
	TObjectPtr<const UUserWidget> StartWidget;

	UPROPERTY()
	TObjectPtr<const UUserWidget> EndWidget;

	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D StartPortDirection;

	UPROPERTY(EditAnywhere, Category = "Spline")
	FVector2D EndPortDirection;

	FVector2D StartPortLocalCoord;
	FVector2D EndPortLocalCoord;
};
