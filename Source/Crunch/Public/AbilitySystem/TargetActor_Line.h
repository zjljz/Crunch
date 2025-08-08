// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_Line.generated.h"

class USphereComponent;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class CRUNCH_API ATargetActor_Line : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ATargetActor_Line();

private:
	UPROPERTY(VisibleAnywhere, Category= "Components")
	TObjectPtr<UNiagaraComponent> LazerVFXComp;

	UPROPERTY(VisibleAnywhere, Category= "Components")
	TObjectPtr<USphereComponent> TargetEndDetectionSphere;

	UPROPERTY()
	TObjectPtr<const AActor> AvatarActor;
};
