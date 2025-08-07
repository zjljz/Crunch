// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CrosshairWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Crunch/CrunchGameplayTags.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Img_Crosshair->SetVisibility(ESlateVisibility::Hidden);

	if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn()))
	{
		OwnerASC->RegisterGameplayTagEvent(CrunchGameplayTags::Stats_Crosshair).AddUObject(this, &ThisClass::OnCrosshairTagUpdate);
		OwnerASC->GenericGameplayEventCallbacks.Add(CrunchGameplayTags::Target_Updated).AddUObject(this, &UCrosshairWidget::OnAimTargetUpdated);
	}

	CrosshairPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	if (!CrosshairPanelSlot)
	{
		UE_LOG(LogTemp, Error, TEXT("Crosshair widget need to be parented under a canvasPanel"));
	}

	OwnerPC = GetOwningPlayer();
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Img_Crosshair->GetVisibility() == ESlateVisibility::Visible)
	{
		UpdateCrosshairPosition();
	}
}

void UCrosshairWidget::OnCrosshairTagUpdate(const FGameplayTag Tag, int32 NewCount)
{
	Img_Crosshair->SetVisibility(NewCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UCrosshairWidget::UpdateCrosshairPosition()
{
	if (!OwnerPC || !CrosshairPanelSlot) return;

	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	int32 SizeX, SizeY;
	OwnerPC->GetViewportSize(SizeX, SizeY);

	if (!AimTarget)
	{
		FVector2D ViewportSize = {(float)SizeX, (float)SizeY};
		CrosshairPanelSlot->SetPosition(ViewportSize / 2.f / ViewportScale);
	}
	else
	{
		FVector2D TargetScreenPos;
		OwnerPC->ProjectWorldLocationToScreen(AimTarget->GetActorLocation(),TargetScreenPos);
		if (TargetScreenPos.X > 0 && TargetScreenPos.Y > 0 && TargetScreenPos.X < SizeX && TargetScreenPos.Y < SizeY)
		{
			CrosshairPanelSlot->SetPosition(TargetScreenPos / ViewportScale);
		}
	}
}

void UCrosshairWidget::OnAimTargetUpdated(const FGameplayEventData* Data)
{
	AimTarget = Data->Target;

	Img_Crosshair->SetColorAndOpacity(AimTarget ? HaveTargetColor : NoTargetColor);
}
