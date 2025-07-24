// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MinionBarrack.h"

#include "AI/MinionCharacter.h"
#include "GameFramework/PlayerStart.h"

AMinionBarrack::AMinionBarrack()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SpawnIntervalHandle, this, &AMinionBarrack::SpawnNewGroup, GroupSpawnInterval, true);
	}
}

APlayerStart* AMinionBarrack::GetNextSpawnSpot()
{
	if (SpawnSpots.Num() == 0)
	{
		//@todo: 添加一个Log.
		return nullptr;
	}

	++NextSpawnSpotIndex;

	if (NextSpawnSpotIndex >= SpawnSpots.Num())
	{
		NextSpawnSpotIndex = 0; // Reset to the first spawn spot if we exceed the number of spots
	}

	return SpawnSpots[NextSpawnSpotIndex];
}

void AMinionBarrack::SpawnNewMinions(int32 Amount)
{
	for (int i = 0; i < Amount; ++i)
	{
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}

		AMinionCharacter* NewMinion = GetWorld()->SpawnActorDeferred<AMinionCharacter>
		(
			MinionClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);

		NewMinion->SetGenericTeamId(BarrackTeamId);
		NewMinion->FinishSpawning(SpawnTransform);
		NewMinion->SetGoal(Goal);
		MinionPool.Add(NewMinion);
	}
}

void AMinionBarrack::SpawnNewGroup()
{
	int i = MinionPerGroup;

	//考虑是否有可用的Minion,如果有 就可以不生成新的Minion 而是复用dead Minion.
	while (i > 0)
	{
		//查找可用的Minion.
		AMinionCharacter* NextAvailableMinion = GetNextAvailableMinion();
		if (!NextAvailableMinion) break;

		//找到SpawnTransform 以及设置 StartSpot.
		FTransform SpawnTransform = GetActorTransform();
		if (APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		
		NextAvailableMinion->RemoveDeadTag(); // 清除DeadTag 来 ReSpawn Minion.
		NextAvailableMinion->SetActorTransform(SpawnTransform);

		--i;
	}

	SpawnNewMinions(i);
}

AMinionCharacter* AMinionBarrack::GetNextAvailableMinion() const
{
	for (AMinionCharacter* Minion : MinionPool)
	{
		if (Minion && Minion->IsDead())
		{
			return Minion;
		}
	}

	return nullptr;
}
