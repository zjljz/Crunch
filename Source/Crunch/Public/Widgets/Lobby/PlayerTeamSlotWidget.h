// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTeamSlotWidget.generated.h"

class UPawnData;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class CRUNCH_API UPlayerTeamSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void UpdateSlot(const FString& PlayerName, const UPawnData* NewPawnData);

private:
	void UpdateNameText() const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_CharacterIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_Hover;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterIconMatParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CharacterEmptyMatParamName = "Empty";

	//缓存的PlayerName 以及他们选择的CharacterName.方便进行切换.
	FString CachedPlayerNameStr = "";
	FString CachedCharacterNameStr = "";
};
