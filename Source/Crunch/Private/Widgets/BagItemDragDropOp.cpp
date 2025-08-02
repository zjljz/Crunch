// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BagItemDragDropOp.h"
#include "Widgets/BagItemWidget.h"

void UBagItemDragDropOp::SetDragItem(UBagItemWidget* DragWidget)
{
	Payload = DragWidget;
	if (DragVisualClass)
	{
		if (UInventoryItemWidget* DragItemWidget = CreateWidget<UInventoryItemWidget>(GetWorld(), DragVisualClass))
		{
			DragItemWidget->SetItemIcon(DragWidget->GetIconTexture());
			DefaultDragVisual = DragItemWidget;
		}
	}
}
