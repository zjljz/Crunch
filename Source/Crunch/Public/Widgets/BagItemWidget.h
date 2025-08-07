// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/InventoryItemWidget.h"
#include "BagItemWidget.generated.h"

class UGameplayAbility;
class UBagItemDragDropOp;
class UTextBlock;
class UBagItemWidget;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBagItemDropped, UBagItemWidget* /* DestinationWidget */, UBagItemWidget* /* SourceWidget */)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnButtonClick, const FInventoryItemHandle& /* ItemHandle */)

/**
 * 用于人物 背包/装备栏的ItemWidget.
 */
UCLASS()
class CRUNCH_API UBagItemWidget : public UInventoryItemWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void UpdateItemWidget(const UInventoryItem* NewItem);

	bool IsEmpty() const;
	void EmptySlot();
	FORCEINLINE int GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int NewIndex) { SlotIndex = NewIndex; }
	FORCEINLINE UTexture2D* GetIconTexture() const;

	void UpdateStackCount();

	FORCEINLINE const UInventoryItem* GetInventoryItem() const { return Item; }
	FInventoryItemHandle GetItemHandle() const;

	void StartCooldown(float CooldownDuration, float TimeRemaining);

	virtual void SetItemIcon(UTexture2D* NewTexture) override;

	//当ASC中有Ability被Commit时调用.
	void OnAbilityCommited(UGameplayAbility* Ability);

private:
	void CooldownFinished();

	void UpdateCooldown();

	void ClearCooldown();

	void UpdateCanCastDisplay(bool bCanCast);

	void BindCanCastDelegate();
	void UnBindCanCastDelegate();
	
protected:
	virtual void OnLeftMouseButtonClicked() override;
	virtual void OnRightMouseButtonClicked() override;

	//当Slate发现某个Widget被Drag(拖动)时 调用该函数.
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	//当用户把something拖动到Widget时调用.
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	FOnBagItemDropped OnBagItemDropped;

	FOnButtonClick OnLeftButtonClicked;
	FOnButtonClick OnRightButtonClicked;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TObjectPtr<UTexture2D> EmptyTexture;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CooldownCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CooldownDuration;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ManaCost;

	//生成的Item.
	UPROPERTY()
	TObjectPtr<const UInventoryItem> Item;

	//代表在 背包/装备栏 的Index;
	int SlotIndex;

	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
	TSubclassOf<UBagItemDragDropOp> DragDropOpClass;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CooldownAmtParamName = "Percent";

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName IconTextureParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CanCastParamName = "CanCast";

	FTimerHandle CooldownDurationTimerHandle;
	FTimerHandle CooldownUpdateTimerHandle;

	float CooldownTimeRemaining = 0.f;
	float CooldownTimeDuration = 0.f;

	FNumberFormattingOptions CooldownNumFormatOp;
};
