// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MatchStatWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/StormCore.h"
#include "Kismet/GameplayStatics.h"

void UMatchStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StormCore = Cast<AStormCore>(UGameplayStatics::GetActorOfClass(this, AStormCore::StaticClass()));
	if (StormCore)
	{
		StormCore->OnTeamInfluenceCountUpdateDelegate.AddUObject(this, &ThisClass::UpdateTeamInfluence);
		StormCore->OnGoalReachedDelegate.AddUObject(this, &ThisClass::OnMatchFinished);
		GetWorld()->GetTimerManager().SetTimer(UpdateProgressTimerHandle, this, &ThisClass::UpdateProgress, ProgressUpdateInterval, true);
	}
}

void UMatchStatWidget::UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount)
{
	Text_TeamOneCount->SetText(FText::AsNumber(TeamOneCount));
	Text_TeamTwoCount->SetText(FText::AsNumber(TeamTwoCount));
}

void UMatchStatWidget::OnMatchFinished(AActor* ViewTarget, int WinTeamId)
{
	float Progress = WinTeamId == 0 ? 1.f : 0.f;
	GetWorld()->GetTimerManager().ClearTimer(UpdateProgressTimerHandle);

	if (Img_Progress)
	{
		Img_Progress->GetDynamicMaterial()->SetScalarParameterValue(ProgressMaterialName, Progress);
	}
}

void UMatchStatWidget::UpdateProgress()
{
	if (StormCore)
	{
		float Progress = StormCore->GetProgress();
		if (Img_Progress)
		{
			Img_Progress->GetDynamicMaterial()->SetScalarParameterValue(ProgressMaterialName, Progress);
		}
	}
}
