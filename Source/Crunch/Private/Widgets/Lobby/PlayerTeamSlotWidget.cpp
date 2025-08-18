// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/PlayerTeamSlotWidget.h"

#include "Character/PawnData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerTeamSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Img_CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyMatParamName, 1);
}

void UPlayerTeamSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Text_Name->SetText(FText::FromString(CachedCharacterNameStr));
	PlayAnimationForward(Anim_Hover);
}

void UPlayerTeamSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Text_Name->SetText(FText::FromString(CachedPlayerNameStr));
	PlayAnimationReverse(Anim_Hover);
}

void UPlayerTeamSlotWidget::UpdateSlot(const FString& PlayerName, const UPawnData* NewPawnData)
{
	CachedPlayerNameStr = PlayerName;

	if (NewPawnData)
	{
		CachedCharacterNameStr = NewPawnData->GetCharacterDisplayName();
		Img_CharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(CharacterIconMatParamName, NewPawnData->GetOrLoadCharacterIcon());
		Img_CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyMatParamName, 0);
	}
	else
	{
		CachedCharacterNameStr = "";
		Img_CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyMatParamName, 1);
	}
	
	UpdateNameText();
}

void UPlayerTeamSlotWidget::UpdateNameText() const
{
	if (IsHovered())
	{
		Text_Name->SetText(FText::FromString(CachedCharacterNameStr));
	}
	else
	{
		Text_Name->SetText(FText::FromString(CachedPlayerNameStr));
	}
}
