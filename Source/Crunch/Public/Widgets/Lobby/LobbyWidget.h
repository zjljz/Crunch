// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/PawnData.h"
#include "Player/PlayerInfoTypes.h"
#include "LobbyWidget.generated.h"

class UPlayerTeamLayoutWidget;
class UAbilityListView;
class ACharacterDisplay;
class ACrunchPlayerState;
class UTileView;
class ACrunchGameState;
class ALobbyPlayerController;
class UTeamSelectionWidget;
class UUniformGridPanel;
class UButton;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class CRUNCH_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
private:
	void ClearAndPopulateTeamSelectionSlots();

	void SlotSelected(uint8 NewSlotId);

	//获取GameState 并绑定到PlayerSelection的Update.
	void ConfigureGameState();

	//当PlayerSelections更新时,更新UI.
	void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections);

	UFUNCTION()
	void OnStartHeroSelectionClicked();

	void SwitchToHeroSelection() const;

	void OnPawnDataLoaded();

	void OnCharacterSelected(UObject* SelectedObj);
	
	void SpawnCharacterDisplay();

	//Lobby选择Pawn时 更新这个Character的相关信息 例如:Ability,Mesh,Anim.
	void UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection) const;

	UFUNCTION()
	void OnStartMatchBtnClicked();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MainSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> TeamSelectionRoot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> HeroSelectionRoot;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_StartHeroSelection;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> TeamSelectionGridPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> CharacterSelectTileView;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPlayerTeamLayoutWidget> PlayerTeamLayoutWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_StartMatch;
	
	UPROPERTY(EditDefaultsOnly, Category = "TeamSelection")
	TSubclassOf<UTeamSelectionWidget> TeamSelectionWidgetClass;

	UPROPERTY()
	TArray<UTeamSelectionWidget*> TeamSelectionSlots;

	UPROPERTY()
	TObjectPtr<ALobbyPlayerController> OwnerPC;

	UPROPERTY()
	TObjectPtr<ACrunchPlayerState> OwnerPS;

	UPROPERTY()
	TObjectPtr<ACrunchGameState> CrunchGameState;

	UPROPERTY(EditDefaultsOnly, Category = "Character Display")
	TSubclassOf<ACharacterDisplay> CharacterDisplayClass;

	UPROPERTY()
	TObjectPtr<ACharacterDisplay> CharacterDisplay;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAbilityListView> AbilityListView;
};