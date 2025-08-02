// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "BagItemDragDropOp.generated.h"

class UInventoryItemWidget;
class UBagItemWidget;
/**
 * 
 */
UCLASS()
class CRUNCH_API UBagItemDragDropOp : public UDragDropOperation
{
	GENERATED_BODY()

public:
	//在拖动时 会有一个Widget跟随鼠标 这里就是为了创建那个Widget.
	void SetDragItem(UBagItemWidget* DragWidget);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<UInventoryItemWidget> DragVisualClass;
};
