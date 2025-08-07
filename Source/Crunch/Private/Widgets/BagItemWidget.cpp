// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BagItemWidget.h"

#include "ShopItemAsset.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/BagItemDragDropOp.h"
#include "Widgets/ItemToolTip.h"

void UBagItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EmptySlot();
}

void UBagItemWidget::UpdateItemWidget(const UInventoryItem* NewItem)
{
	UnBindCanCastDelegate();

	Item = NewItem;
	if (!Item || !Item->IsValid() || Item->GetStackCount() <= 0)
	{
		EmptySlot();
		return;
	}

	SetItemIcon(NewItem->GetShopItem()->GetItemIcon());
	if (UItemToolTip* ToolTip = CreateToolTipWidget(NewItem->GetShopItem()))
	{
		ToolTip->SetPrice(NewItem->GetShopItem()->GetSellPrice());
	}

	if (Item->GetShopItem()->GetIsStackable())
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		UpdateStackCount();
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Hidden);
	}

	ClearCooldown();
	if (Item->IsGrantedAnyAbility())
	{
		UpdateCanCastDisplay(Item->CanCastAbility());
		float AbilityCooldownRemaining = Item->GetAbilityCooldownTimeRemaining();
		float AbilityCooldownDuration = Item->GetAbilityCooldownDuration();

		if (AbilityCooldownRemaining > 0.f)
		{
			StartCooldown(AbilityCooldownDuration, AbilityCooldownRemaining);
		}

		float AbilityManaCost = Item->GetAbilityManaCost();
		Text_ManaCost->SetVisibility(AbilityManaCost > 0.f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		Text_ManaCost->SetText(FText::AsNumber(AbilityManaCost));

		Text_CooldownDuration->SetVisibility(AbilityCooldownDuration > 0.f ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		Text_CooldownDuration->SetText(FText::AsNumber(AbilityCooldownDuration));
		BindCanCastDelegate();
	}
	else
	{
		Text_ManaCost->SetVisibility(ESlateVisibility::Hidden);
		Text_CooldownDuration->SetVisibility(ESlateVisibility::Hidden);
		Text_CooldownCount->SetVisibility(ESlateVisibility::Hidden);
		UpdateCanCastDisplay(true);
	}
}

bool UBagItemWidget::IsEmpty() const
{
	return !Item || !Item->IsValid();
}

void UBagItemWidget::EmptySlot()
{
	ClearCooldown();
	UnBindCanCastDelegate();
	Item = nullptr;
	
	SetItemIcon(EmptyTexture);
	SetToolTip(nullptr);

	Text_StackCount->SetVisibility(ESlateVisibility::Hidden);
	Text_CooldownCount->SetVisibility(ESlateVisibility::Hidden);
	Text_CooldownDuration->SetVisibility(ESlateVisibility::Hidden);
	Text_ManaCost->SetVisibility(ESlateVisibility::Hidden);
}

UTexture2D* UBagItemWidget::GetIconTexture() const
{
	if (Item && Item->GetShopItem())
	{
		return Item->GetShopItem()->GetItemIcon();
	}

	return nullptr;
}

void UBagItemWidget::UpdateStackCount()
{
	if (Item)
	{
		Text_StackCount->SetText(FText::AsNumber(Item->GetStackCount()));
	}
}

FInventoryItemHandle UBagItemWidget::GetItemHandle() const
{
	if (!IsEmpty())
	{
		return Item->GetHandle();
	}

	return FInventoryItemHandle::InvalidHandle();
}

void UBagItemWidget::StartCooldown(float CooldownDuration, float TimeRemaining)
{
	CooldownTimeRemaining = TimeRemaining;
	CooldownTimeDuration = CooldownDuration;

	GetWorld()->GetTimerManager().SetTimer(CooldownDurationTimerHandle, this, &UBagItemWidget::CooldownFinished, CooldownTimeRemaining);
	GetWorld()->GetTimerManager().SetTimer(CooldownUpdateTimerHandle, this, &ThisClass::UpdateCooldown, CooldownUpdateInterval, true);

	Text_CooldownCount->SetVisibility(ESlateVisibility::Visible);
}

void UBagItemWidget::SetItemIcon(UTexture2D* NewTexture)
{
	if (GetItemIcon())
	{
		GetItemIcon()->GetDynamicMaterial()->SetTextureParameterValue(IconTextureParamName, NewTexture);
		return;
	}

	Super::SetItemIcon(NewTexture);
}

void UBagItemWidget::OnAbilityCommited(UGameplayAbility* Ability)
{
	//说明Commite的Ability就是当前Item的GrantedAbility
	if (!Item || !Item->IsValid()) return;

	if (Ability->GetClass() == Item->GetShopItem()->GetGrantedAbility())
	{
		StartCooldown(Item->GetAbilityCooldownDuration(), Item->GetAbilityCooldownTimeRemaining());
	}
}

void UBagItemWidget::CooldownFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(CooldownDurationTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(CooldownUpdateTimerHandle);

	Text_CooldownCount->SetVisibility(ESlateVisibility::Hidden);
	if (GetItemIcon())
	{
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(CooldownAmtParamName, 1.f);
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(CanCastParamName, 1.f);
	}
}

void UBagItemWidget::UpdateCooldown()
{
	CooldownTimeRemaining -= CooldownUpdateInterval;
	float CooldownAmt = 1.f - CooldownTimeRemaining / CooldownTimeDuration;
	CooldownNumFormatOp.MaximumFractionalDigits = CooldownTimeRemaining > 1.f ? 0 : 2;
	Text_CooldownCount->SetText(FText::AsNumber(CooldownTimeRemaining, &CooldownNumFormatOp));

	if (GetItemIcon())
	{
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(CooldownAmtParamName, CooldownAmt);
	}
}

void UBagItemWidget::ClearCooldown()
{
	CooldownFinished();
}

void UBagItemWidget::UpdateCanCastDisplay(bool bCanCast)
{
	GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(CanCastParamName, bCanCast ? 1.f : 0.f);
}

void UBagItemWidget::BindCanCastDelegate()
{
	if (Item)
	{
		const_cast<UInventoryItem*>(Item.Get())->OnAbilityCanCastUpdate.AddUObject(this, &ThisClass::UpdateCanCastDisplay);
	}
}

void UBagItemWidget::UnBindCanCastDelegate()
{
	if (Item)
	{
		const_cast<UInventoryItem*>(Item.Get())->OnAbilityCanCastUpdate.RemoveAll(this);
	}
}

void UBagItemWidget::OnLeftMouseButtonClicked()
{
	if (!IsEmpty())
	{
		OnLeftButtonClicked.Broadcast(GetItemHandle());
	}
}

void UBagItemWidget::OnRightMouseButtonClicked()
{
	if (!IsEmpty())
	{
		OnRightButtonClicked.Broadcast(GetItemHandle());
	}
}

void UBagItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!IsEmpty() && DragDropOpClass)
	{
		if (UBagItemDragDropOp* DragDropOp = NewObject<UBagItemDragDropOp>(this, DragDropOpClass))
		{
			DragDropOp->SetDragItem(this);
			OutOperation = DragDropOp;
		}
	}
}

bool UBagItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	//这里先获取被拖动的那个Widget.
	if (UBagItemWidget* OtherWidget = Cast<UBagItemWidget>(InOperation->Payload))
	{
		if (OtherWidget && !OtherWidget->IsEmpty())
		{
			OnBagItemDropped.Broadcast(this, OtherWidget);
			return true;
		}
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
