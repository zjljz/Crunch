// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

class UShopItemAsset;
class UItemToolTip;
class UImage;

/**
 *  库存系统 用于Item在 Shop/Bag/装备栏 中的Widget基类, 自带一个Img显示物品图片以及TooltipWidget.
 */
UCLASS()
class CRUNCH_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void SetItemIcon(UTexture2D* NewTexture);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void OnLeftMouseButtonClicked();
	virtual void OnRightMouseButtonClicked();

	//创建ToolTip,初始化,并设置到Tooltip.
	UItemToolTip* CreateToolTipWidget(const UShopItemAsset* Item);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_ItemIcon;

	UPROPERTY(EditDefaultsOnly, Category = "ToolTip")
	TSubclassOf<UItemToolTip> ToolTipWidgetClass;
};
