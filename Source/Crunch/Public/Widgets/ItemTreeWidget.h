// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTreeWidget.generated.h"

class UCanvasPanel;
class UCanvasPanelSlot;
class ITreeNodeInterface;
/**
 * 
 */
UCLASS()
class CRUNCH_API UItemTreeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void DrawFromNode(const ITreeNodeInterface* NodeInterface);

private:
	/**
	 * 
	 * @param bUpStream 是向上递归,还是向下递归.
	 * @param StartNodeDepth 递归的深度.
	 * @param NextLeafXPos 
	 * @param OutStreamSlots 
	 */
	void DrawStream(bool bUpStream, const ITreeNodeInterface* StartNodeInterface, UUserWidget* StartNodeWidget, UCanvasPanelSlot* StartNodeSlot,
	                int StartNodeDepth, float& NextLeafXPos, TArray<UCanvasPanelSlot*>& OutStreamSlots);

	void ClearTree();

	//获取Node对应的Widget 并添加到RootPanel,设置PanelSlot.
	UUserWidget* CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot*& OutCanvasSlot);

	//创建俩个Widget之间的连线.
	void CreateConnection(const UUserWidget* FromWidget, UUserWidget* ToWidget);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RootPanel;

	UPROPERTY()
	TObjectPtr<const UObject> CurCenterItem;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D NodeSize = FVector2D{60.f};

	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D NodeGap = FVector2D{16.f, 30.f};

	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FLinearColor ConnectionColor = FLinearColor{0.8f, 0.8f, 0.8f, 1.f};

	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	float ConnectionThickness = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D SourcePortLocalPos = FVector2D{0.5f, 0.9f};

	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D DestinationPortLocalPos = FVector2D{0.5f, 0.1f};

	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D SourcePortDirection = FVector2D{0.f, 90.f};

	UPROPERTY(EditDefaultsOnly, Category = "Tree")
	FVector2D DestinationPortDirection = FVector2D{0.f, 90.f};
};
