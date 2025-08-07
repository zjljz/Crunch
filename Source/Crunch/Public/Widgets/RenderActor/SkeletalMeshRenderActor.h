// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/RenderActor/RenderActor.h"
#include "SkeletalMeshRenderActor.generated.h"

/**
 * 
 */
UCLASS()
class CRUNCH_API ASkeletalMeshRenderActor : public ARenderActor
{
	GENERATED_BODY()

public:
	ASkeletalMeshRenderActor();

protected:
	virtual void BeginPlay() override;

public:
	void ConfigureSkeletalMesh(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> AnimBP);

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "SkeletalMesh Render Actor")
	TObjectPtr<USkeletalMeshComponent> MeshComp;
};
