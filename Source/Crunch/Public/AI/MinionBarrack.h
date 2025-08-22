// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "MinionBarrack.generated.h"

class AMinionCharacter;
class APlayerStart;

UCLASS()
class CRUNCH_API AMinionBarrack : public AActor
{
	GENERATED_BODY()

public:
	AMinionBarrack();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	APlayerStart* GetNextSpawnSpot();

	void SpawnNewMinions(int32 Amount);

	// 生成新一组小兵.
	void SpawnNewGroup();

	AMinionCharacter* GetNextAvailableMinion() const;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	int32 NextSpawnSpotIndex = -1;

	//Spawn minions amount in group.
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	int32 MinionPerGroup = 5;
	
	//Time in seconds between spawn.
	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	float GroupSpawnInterval = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TObjectPtr<AActor> Goal;
	
private:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FGenericTeamId BarrackTeamId;
	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AMinionCharacter> MinionClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<APlayerStart*> SpawnSpots;

	FTimerHandle SpawnIntervalHandle;
	
	UPROPERTY()
	TArray<AMinionCharacter*> MinionPool;
};
