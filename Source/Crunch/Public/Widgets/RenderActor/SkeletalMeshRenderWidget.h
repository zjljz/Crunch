// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/RenderActor/RenderActorWidget.h"
#include "SkeletalMeshRenderWidget.generated.h"

class ASkeletalMeshRenderActor;
/**
 * 
 */
UCLASS()
class CRUNCH_API USkeletalMeshRenderWidget : public URenderActorWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	virtual void SpawnRenderActor() override;
	virtual ARenderActor* GetRenderActor() const override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Skeletal Mesh Redner")
	TSubclassOf<ASkeletalMeshRenderActor> RenderActorClass;

	UPROPERTY()
	TObjectPtr<ASkeletalMeshRenderActor> RenderActor;
};
