// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemTreeWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Widgets/SplineWidget.h"
#include "Widgets/TreeNodeInterface.h"

void UItemTreeWidget::DrawFromNode(const ITreeNodeInterface* NodeInterface)
{
	if (!NodeInterface) return;

	if (CurCenterItem == NodeInterface->GetItemObject()) return;

	ClearTree();
	CurCenterItem = NodeInterface->GetItemObject();

	float NextLeafXPos = 0.f;
	UCanvasPanelSlot* CenterWidgetSlot = nullptr;
	UUserWidget* CenterWidget = CreateWidgetForNode(NodeInterface, CenterWidgetSlot);
	TArray<UCanvasPanelSlot*> LowerStreamSlots, UpperStreamSlots;

	DrawStream(false, NodeInterface, CenterWidget, CenterWidgetSlot, 0, NextLeafXPos, LowerStreamSlots);
	float LowerStreamXMax = NextLeafXPos - NodeSize.X - NodeGap.X;
	float LowerMoveAmt = 0.f - LowerStreamXMax / 2.f;
	for (UCanvasPanelSlot* StreamSlot : LowerStreamSlots)
	{
		StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D{LowerMoveAmt, 0.f});
	}
	
	NextLeafXPos = 0.f;
	DrawStream(true, NodeInterface, CenterWidget, CenterWidgetSlot, 0, NextLeafXPos, UpperStreamSlots);
	float UpperStreamXMax = NextLeafXPos - NodeSize.X - NodeGap.X;
	float UpperMoveAmt = 0.f - UpperStreamXMax / 2.f;
	for (UCanvasPanelSlot* StreamSlot : UpperStreamSlots)
	{
		StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D{UpperMoveAmt, 0.f});
	}
	
	CenterWidgetSlot->SetPosition(FVector2D::Zero());
}

void UItemTreeWidget::DrawStream(bool bUpStream, const ITreeNodeInterface* StartNodeInterface, UUserWidget* StartNodeWidget, UCanvasPanelSlot* StartNodeSlot,
                                 int StartNodeDepth, float& NextLeafXPos, TArray<UCanvasPanelSlot*>& OutStreamSlots)
{
	TArray<const ITreeNodeInterface*> NextTreeNodeInterfaces = bUpStream ? StartNodeInterface->GetInputs() : StartNodeInterface->GetOutputs();
	float StartNodeYPos = (NodeSize.Y + NodeGap.Y) * StartNodeDepth * (bUpStream ? -1 : 1);

	//这个节点是一个Leaf节点,或者说 这个Item 没有Combination/Ingredient.
	if (NextTreeNodeInterfaces.Num() == 0)
	{
		StartNodeSlot->SetPosition(FVector2D(NextLeafXPos, StartNodeYPos));
		NextLeafXPos += (NodeGap.X + NodeSize.X);
		return;
	}

	float NextNodeXPosSum = 0;
	for (const ITreeNodeInterface* NextTreeNodeInterface : NextTreeNodeInterfaces)
	{
		UCanvasPanelSlot* NextNodeWidgetSlot;
		UUserWidget* NextWidget = CreateWidgetForNode(NextTreeNodeInterface, NextNodeWidgetSlot);
		OutStreamSlots.Add(NextNodeWidgetSlot);
		if (bUpStream)
		{
			CreateConnection(NextWidget, StartNodeWidget);
		}
		else
		{
			CreateConnection(StartNodeWidget, NextWidget);
		}

		DrawStream(bUpStream, NextTreeNodeInterface, NextWidget, NextNodeWidgetSlot, StartNodeDepth + 1, NextLeafXPos, OutStreamSlots);
		NextNodeXPosSum += NextNodeWidgetSlot->GetPosition().X;
	}

	float StartNodeXPos = NextNodeXPosSum / NextTreeNodeInterfaces.Num();
	StartNodeSlot->SetPosition(FVector2D{StartNodeXPos, StartNodeYPos});
}

void UItemTreeWidget::ClearTree()
{
	RootPanel->ClearChildren();
}

UUserWidget* UItemTreeWidget::CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot*& OutCanvasSlot)
{
	if (!Node) return nullptr;

	UUserWidget* NodeWidget = Node->GetWidget();
	OutCanvasSlot = RootPanel->AddChildToCanvas(NodeWidget);
	if (OutCanvasSlot)
	{
		OutCanvasSlot->SetSize(NodeSize);
		OutCanvasSlot->SetAnchors(FAnchors(0.5f));
		OutCanvasSlot->SetAlignment(FVector2D(0.5f));
		OutCanvasSlot->SetZOrder(1);
	}

	return NodeWidget;
}

void UItemTreeWidget::CreateConnection(const UUserWidget* FromWidget, UUserWidget* ToWidget)
{
	if (!FromWidget || !ToWidget)
	{
		return;
	}

	//创建连接线,添加到插槽,并初始化.
	USplineWidget* ConnectSplineWidget = CreateWidget<USplineWidget>(GetOwningPlayer());
	if (UCanvasPanelSlot* ConnectionCanvasSlot = RootPanel->AddChildToCanvas(ConnectSplineWidget))
	{
		ConnectionCanvasSlot->SetAnchors(FAnchors{0.f});
		ConnectionCanvasSlot->SetAlignment(FVector2D{0.f});
		ConnectionCanvasSlot->SetPosition(FVector2D{0.f});
		ConnectionCanvasSlot->SetZOrder(0);
	}

	ConnectSplineWidget->SetupSpline(FromWidget, ToWidget, SourcePortLocalPos, DestinationPortLocalPos,
	                                 SourcePortDirection, DestinationPortDirection);

	ConnectSplineWidget->SetSplineStyle(ConnectionColor, ConnectionThickness);
}
