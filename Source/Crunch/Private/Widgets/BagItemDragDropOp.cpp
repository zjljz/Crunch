// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BagItemDragDropOp.h"
#include "Widgets/BagItemWidget.h"

void UBagItemDragDropOp::SetDragItem(UBagItemWidget* DragWidget)
{
	//这里的作用是 在拖动时 会有一个Widget跟随鼠标 这里就是为了创建那个Widget.
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
