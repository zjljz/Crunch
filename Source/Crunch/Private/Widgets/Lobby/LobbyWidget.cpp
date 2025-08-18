// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Lobby/LobbyWidget.h"

#include "Components/Button.h"
#include "Components/TileView.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/CrunchAssetManager.h"
#include "Framework/CrunchGameState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Lobby/TeamSelectionWidget.h"
#include "Network/CrunchNetStatics.h"
#include "Player/CrunchPlayerState.h"
#include "Player/LobbyPlayerController.h"
#include "Widgets/AbilityListView.h"
#include "Widgets/Lobby/CharacterDisplay.h"
#include "Widgets/Lobby/CharacterEntryWidget.h"
#include "Widgets/Lobby/PlayerTeamLayoutWidget.h"


void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ClearAndPopulateTeamSelectionSlots();

	OwnerPC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (OwnerPC)
	{
		OwnerPC->OnSwitchToHeroSelection.BindUObject(this, &ThisClass::SwitchToHeroSelection);
	}

	ConfigureGameState();
	Btn_StartHeroSelection->SetIsEnabled(false);
	Btn_StartHeroSelection->OnClicked.AddDynamic(this, &ThisClass::OnStartHeroSelectionClicked);

	Btn_StartMatch->SetIsEnabled(false);
	Btn_StartMatch->OnClicked.AddDynamic(this, &ThisClass::OnStartMatchBtnClicked);
	
	UCrunchAssetManager::Get().LoadPawnData(FStreamableDelegate::CreateUObject(this, &ThisClass::OnPawnDataLoaded));
	if (CharacterSelectTileView)
	{
		CharacterSelectTileView->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnCharacterSelected);
	}

	SpawnCharacterDisplay();
}

void ULobbyWidget::ClearAndPopulateTeamSelectionSlots()
{
	TeamSelectionGridPanel->ClearChildren();

	int MaxCount = UCrunchNetStatics::GetPlayerCountPerTeam();

	for (int i = 0; i < MaxCount * 2; ++i)
	{
		if (UTeamSelectionWidget* NewWidget = CreateWidget<UTeamSelectionWidget>(GetOwningPlayer(), TeamSelectionWidgetClass))
		{
			NewWidget->SetSlotId(i);
			TeamSelectionGridPanel->AddChildToUniformGrid(NewWidget, i % MaxCount, i / MaxCount);
			NewWidget->OnSlotClicked.AddUObject(this, &ThisClass::SlotSelected);
			TeamSelectionSlots.Add(NewWidget);
		}
	}
}

void ULobbyWidget::SlotSelected(uint8 NewSlotId)
{
	if (OwnerPC)
	{
		OwnerPC->Server_RequestSlotSelectionChange(NewSlotId);
	}
}

void ULobbyWidget::ConfigureGameState()
{
	if (UWorld* World = GetWorld())
	{
		CrunchGameState = World->GetGameState<ACrunchGameState>();
		if (!CrunchGameState)
		{
			FTimerHandle Handle;
			World->GetTimerManager().SetTimer(Handle, this, &ThisClass::ConfigureGameState, 1.f);
		}
		else
		{
			CrunchGameState->OnPlayerSelectionUpdate.AddUObject(this, &ThisClass::UpdatePlayerSelectionDisplay);
			UpdatePlayerSelectionDisplay(CrunchGameState->GetPlayerSelectionArray());
		}
	}
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (UTeamSelectionWidget* Widget : TeamSelectionSlots)
	{
		Widget->UpdateSlotInfo("Empty");
	}

	for (UUserWidget* CharacterEntry : CharacterSelectTileView->GetDisplayedEntryWidgets())
	{
		if (UCharacterEntryWidget* EntryWidget = Cast<UCharacterEntryWidget>(CharacterEntry))
		{
			EntryWidget->SetSelected(false);
		}
	}

	UCharacterEntryWidget* CurEntry = nullptr;
	for (const FPlayerSelection& Selection : PlayerSelections)
	{
		if (Selection.IsValid())
		{
			TeamSelectionSlots[Selection.GetPlayerSlot()]->UpdateSlotInfo(Selection.GetPlayerNickName());

			CurEntry = CharacterSelectTileView->GetEntryWidgetFromItem<UCharacterEntryWidget>(Selection.GetSelectedPawnData());
			if (CurEntry)
			{
				CurEntry->SetSelected(true);
			}

			if (Selection.IsForPlayer(GetOwningPlayerState()))
			{
				UpdateCharacterDisplay(Selection);
			}
		}
	}

	if (CrunchGameState)
	{
		Btn_StartHeroSelection->SetIsEnabled(CrunchGameState->CanStartHeroSelection());

		Btn_StartMatch->SetIsEnabled(CrunchGameState->CanStartMatch());
	}

	if (PlayerTeamLayoutWidget)
	{
		PlayerTeamLayoutWidget->UpdatePlayerSelection(PlayerSelections);
	}
}

void ULobbyWidget::OnStartHeroSelectionClicked()
{
	if (OwnerPC)
	{
		OwnerPC->Server_StartHeroSelection();
	}
}

void ULobbyWidget::SwitchToHeroSelection() const
{
	MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}

void ULobbyWidget::OnPawnDataLoaded()
{
	TArray<UPawnData*> LoadedPawnData;

	if (UCrunchAssetManager::Get().GetLoadedPawnData(LoadedPawnData))
	{
		CharacterSelectTileView->SetListItems(LoadedPawnData);
	}
}

void ULobbyWidget::OnCharacterSelected(UObject* SelectedObj)
{
	if (!OwnerPS)
	{
		OwnerPS = GetOwningPlayerState<ACrunchPlayerState>();
	}

	if (!OwnerPS) return;

	if (const UPawnData* PawnData = Cast<UPawnData>(SelectedObj))
	{
		OwnerPS->Server_SetSelectedPawnData(PawnData);
	}
}

void ULobbyWidget::SpawnCharacterDisplay()
{
	if (CharacterDisplay || !CharacterDisplayClass) return;

	FTransform CharacterDisplayTransform = FTransform::Identity;
	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
	{
		CharacterDisplayTransform = PlayerStart->GetActorTransform();
	}

	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CharacterDisplay = GetWorld()->SpawnActor<ACharacterDisplay>(CharacterDisplayClass, CharacterDisplayTransform, SpawnParam);
	GetOwningPlayer()->SetViewTarget(CharacterDisplay);
}

void ULobbyWidget::UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection) const
{
	if (!PlayerSelection.GetSelectedPawnData()) return;

	CharacterDisplay->ConfigureFromPawnData(PlayerSelection.GetSelectedPawnData());

	AbilityListView->ClearListItems();
	
	if (const TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>* Abilities = PlayerSelection.GetSelectedPawnData()->GetCharacterAbilities())
	{
		AbilityListView->ConfigureAbilities(*Abilities);
	}
}

void ULobbyWidget::OnStartMatchBtnClicked()
{
	if (OwnerPC)
	{
		OwnerPC->Server_RequestStartMatch();
	}
}
