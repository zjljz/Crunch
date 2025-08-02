// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/SplineWidget.h"

void USplineWidget::SetupSpline(const UUserWidget* InStartWidget, const UUserWidget* InEndWidget, FVector2D& InStartPortLocalCoord, FVector2D& InEndPortLocalCoord, FVector2D& InStartPortDirection,
                                FVector2D& InEndPortDirection)
{
	StartWidget = InStartWidget;
	EndWidget = InEndWidget;
	StartPortLocalCoord = InStartPortLocalCoord;
	EndPortLocalCoord = InEndPortLocalCoord;
	StartPortDirection = InStartPortDirection;
	EndPortDirection = InEndPortDirection;
}

void USplineWidget::SetSplineStyle(const FLinearColor& InColor, float InThickness)
{
	Color = InColor;
	Thickness = InThickness;
}

int32 USplineWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                 const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	LayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FVector2D StartPos = TestStartPos;
	FVector2D EndPos = TestEndPos;

	if (StartWidget && EndWidget)
	{
		StartPos = StartWidget->GetCachedGeometry().GetLocalPositionAtCoordinates(StartPortLocalCoord);
		EndPos = EndWidget->GetCachedGeometry().GetLocalPositionAtCoordinates(EndPortLocalCoord);
	}

	FSlateDrawElement::MakeSpline(
		OutDrawElements,
		++LayerId,
		AllottedGeometry.ToPaintGeometry(),
		StartPos, StartPortDirection,
		EndPos, EndPortDirection,
		Thickness, ESlateDrawEffect::None, Color);

	return LayerId;
}
