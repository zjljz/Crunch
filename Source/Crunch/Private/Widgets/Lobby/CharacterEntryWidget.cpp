// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/CharacterEntryWidget.h"

#include "Character/PawnData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CurPawnData = Cast<UPawnData>(ListItemObject);
	if (CurPawnData)
	{
		Img_CharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(IconTextureMatParamName, CurPawnData->GetOrLoadCharacterIcon());
		Text_CharacterName->SetText(FText::FromString(CurPawnData->GetCharacterDisplayName()));
	}
}

void UCharacterEntryWidget::SetSelected(bool bIsSelected) const
{
	if (CurPawnData)
	{
		Img_CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(SaturationMatParamName, bIsSelected ? 0.f : 1.f);
	}
}
