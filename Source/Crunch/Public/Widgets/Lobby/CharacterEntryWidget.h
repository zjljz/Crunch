// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CharacterEntryWidget.generated.h"

class UPawnData;
class UTextBlock;
class UImage;

/**
 * LobbyWidget中ListView的子项.
 */
UCLASS()
class CRUNCH_API UCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void SetSelected(bool bIsSelected) const;

	FORCEINLINE const UPawnData* GetPawnData() const { return CurPawnData; }

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CharacterName;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName IconTextureMatParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName SaturationMatParamName = "Saturation";

	UPROPERTY()
	TObjectPtr<const UPawnData> CurPawnData;
};
