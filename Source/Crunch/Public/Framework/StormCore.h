// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StormCore.generated.h"

class UCameraComponent;
class AAIController;
class USphereComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGoalReachedDelegate, AActor* /* ViewTarget */, int /* WinTeamId */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTeamInfluenceCountUpdateDelegate, int /* TeamOneInfluenceCount */, int /* TeamTwoInfluenceCount */);

UCLASS()
class CRUNCH_API AStormCore : public ACharacter
{
	GENERATED_BODY()

public:
	AStormCore();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

public:
	float GetProgress() const;
	
private:
	UFUNCTION()
	void NewInfluenceInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void InfluenceLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateTeamWeight();

	void UpdateGoal();

	void OnGoalReached(int WinTeamId);

	void CaptureCore();

	void ExpandFinished();
	
	UFUNCTION()
	void OnRep_CoreToCapture();

public:
	FOnGoalReachedDelegate OnGoalReachedDelegate;
	FOnTeamInfluenceCountUpdateDelegate OnTeamInfluenceCountUpdateDelegate;
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	TObjectPtr<USphereComponent> InfluenceSphereComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	TObjectPtr<UDecalComponent> GroundDecalComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	TObjectPtr<UCameraComponent> ViewCameraComp;
	
	UPROPERTY(EditAnywhere, Category = "Team")
	TObjectPtr<AActor> TeamOneGoal;

	UPROPERTY(EditAnywhere, Category = "Team")
	TObjectPtr<AActor> TeamOneCore;
	
	UPROPERTY(EditAnywhere, Category = "Team")
	TObjectPtr<AActor> TeamTwoGoal;
	
	UPROPERTY(EditAnywhere, Category = "Team")
	TObjectPtr<AActor> TeamTwoCore;

	UPROPERTY(ReplicatedUsing = OnRep_CoreToCapture)
	TObjectPtr<AActor> CoreToCapture;
	
	UPROPERTY()
	TObjectPtr<AAIController> OwnerAIC;
	
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float MaxMoveSpeed = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float InfluenceRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	TObjectPtr<UAnimMontage> ExpandMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	TObjectPtr<UAnimMontage> CaptureMontage;

	float CoreCaptureSpeed = 0.f;
	float TravelLength = 0.f;
	
	int TeamOneInfluenceCount = 0;
	int TeamTwoInfluenceCount = 0;
	float TeamWeight = 0.f;
};
