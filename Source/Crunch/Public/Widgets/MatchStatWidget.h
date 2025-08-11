// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchStatWidget.generated.h"

class AStormCore;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class CRUNCH_API UMatchStatWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

private:

	void UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount);

	void OnMatchFinished(AActor* ViewTarget, int WinTeamId);

	void UpdateProgress();
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_TeamOneCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_TeamTwoCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Progress;

	UPROPERTY()
	TObjectPtr<AStormCore> StormCore;

	UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
	float ProgressUpdateInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
	FName ProgressMaterialName = "Progress";
	
	FTimerHandle UpdateProgressTimerHandle;
};
