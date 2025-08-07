// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RenderActor/SkeletalMeshRenderWidget.h"

#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Character.h"
#include "Widgets/RenderActor/RenderActorTargetInterface.h"
#include "Widgets/RenderActor/SkeletalMeshRenderActor.h"

void USkeletalMeshRenderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* PlayerCharacter = GetOwningPlayerPawn<ACharacter>();
	if (PlayerCharacter && RenderActor)
	{
		RenderActor->ConfigureSkeletalMesh(PlayerCharacter->GetMesh()->GetSkeletalMeshAsset(), PlayerCharacter->GetMesh()->GetAnimClass());

		USceneCaptureComponent2D* CaptureComp = RenderActor->GetCaptureComponent();
		if (IRenderActorTargetInterface* RenderTargetInterface = Cast<IRenderActorTargetInterface>(PlayerCharacter))
		{
			if (CaptureComp)
			{
				CaptureComp->SetRelativeLocation(RenderTargetInterface->GetCaptureLocalPosition());
				CaptureComp->SetRelativeRotation(RenderTargetInterface->GetCaptureLocalRotation());
			}
		}
	}
}

void USkeletalMeshRenderWidget::SpawnRenderActor()
{
	if (!RenderActorClass) return;

	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		RenderActor = World->SpawnActor<ASkeletalMeshRenderActor>(RenderActorClass, SpawnParam);
	}
}

ARenderActor* USkeletalMeshRenderWidget::GetRenderActor() const
{
	return RenderActor;
}
