// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "TargetActor_Line.generated.h"

class USphereComponent;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class CRUNCH_API ATargetActor_Line : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ATargetActor_Line();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return OwnerTeamId; }
	FORCEINLINE virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { OwnerTeamId = NewTeamID; }

public:
	void ConfigureTargetSetting(float NewTargetRange, float NewDetectionCylinderRadius, float NewTargetingInterval, FGenericTeamId NewTeamId, bool NewbDrawDebug);

private:
	void DoTargetCheckAndReport();

	void UpdateTargetTrack();

	bool ShouldReportActorAsTarget(const AActor* InActor) const;
private:
	UPROPERTY(VisibleAnywhere, Category= "Components")
	TObjectPtr<UNiagaraComponent> LazerVFXComp;

	UPROPERTY(VisibleAnywhere, Category= "Components")
	TObjectPtr<USphereComponent> TargetEndDetectionSphere;

	UPROPERTY(Replicated)
	TObjectPtr<const AActor> AvatarActor;

	UPROPERTY(Replicated)
	float TargetRange;

	UPROPERTY(Replicated)
	float DetectionCylinderRadius;

	UPROPERTY()
	float TargetingInterval;

	UPROPERTY(Replicated)
	FGenericTeamId OwnerTeamId;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FName LazerFXLengthParamName = "Length";
	
	UPROPERTY()
	bool bShouldDrawDebug = false;

	FTimerHandle PeriodicalTargetingTimerHandle;
};
