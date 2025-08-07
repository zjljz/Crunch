// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "ProjectileActor.generated.h"

UCLASS()
class CRUNCH_API AProjectileActor : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AProjectileActor();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	FORCEINLINE virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override { TeamId = NewTeamId; }
	FORCEINLINE virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

protected:
	virtual void BeginPlay() override;

public:
	void ShootProjectile(float InSpeed, float InMaxDistance, const AActor* InTarget, FGenericTeamId InTeamId, FGameplayEffectSpecHandle InHitEffectHandle);

private:
	void TravelMaxDistanceReached();

	//这里是仿照
	void SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult) const;

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	UPROPERTY(Replicated)
	FVector MoveDirection;

	UPROPERTY(Replicated)
	float ProjectileSpeed;

	UPROPERTY()
	TObjectPtr<const AActor> TargetActor;

	UPROPERTY(EditDefaultsOnly, Category= "AbilitySystem")
	FGameplayTag HitCueTag;

	FGameplayEffectSpecHandle HitEffectSpecHandle;

	FTimerHandle ShootTimeHandle;
};
