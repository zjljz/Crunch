// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "TargetActor_BlackHole.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class CRUNCH_API ATargetActor_BlackHole : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ATargetActor_BlackHole();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void ConfirmTargetingAndContinue() override;
	virtual void CancelTargeting() override;
	
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamId = NewTeamID; }

public:
	void ConfigureBlackHole(float InRange, float InPullSpeed, float InDuration, const FGenericTeamId& InTeamId);

private:
	UFUNCTION()
	void OnActorInBlackHoleRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnActorLeftBlackHoleRange(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TryAddTarget(AActor* Target);

	void RemoveTarget(AActor* Target);

	void StopBlackHole();
	
	UFUNCTION()
	void OnRep_BlackHoleRange();

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	TObjectPtr<USphereComponent> DetectionSphereComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Component")
	TObjectPtr<UParticleSystemComponent> VFXComp;

	UPROPERTY(EditDefaultsOnly, Category= "VFX")
	TObjectPtr<UNiagaraSystem> BlackHoleLinkVFX;

	UPROPERTY(EditDefaultsOnly, Category= "VFX")
	FName BlackHoleVFXOriginName = "Origin";

	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY(ReplicatedUsing = OnRep_BlackHoleRange)
	float BlackHoleRange = 0.f;

	UPROPERTY()
	float BlackHolePullSpeed = 0.f;

	UPROPERTY()
	float BlackHoleDuration = 0.f;

	UPROPERTY()
	TMap<AActor*, UNiagaraComponent*> ActorsInRangeMap;

	FTimerHandle BlackHoleDurationTimerHandle;
};
