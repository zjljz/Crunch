// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "CrunchCharacter.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UWidgetComponent;
class UCrunchAttributeSet;
class UCrunchAbilitySystemComponent;

UCLASS()
class CRUNCH_API ACrunchCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ACrunchCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//这个函数只在Server上调用.
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;

public:
	/***************************************************
	 *           Gameplay Ability System
	 **************************************************/

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void ServerInit();
	void ClientInit();

	void BindGASChangeDelegate();

	void DeadTagUpDate(const FGameplayTag Tag, int32 NewCount);

private:
	UPROPERTY(VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UCrunchAbilitySystemComponent> CrunchASC;

	UPROPERTY(VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UCrunchAttributeSet> CrunchAttributeSet;


	/***************************************************
	 *           UI Widgets
	 **************************************************/
private:
	//为OverHeadWidget配置ASC.
	void ConfigureOverHeadWidget();

	//定时 检测是否需要显示头顶UI.
	void UpdateHeadGaugeVisibility();
	UPROPERTY(VisibleAnywhere, Category = "UI | OverHead")
	TObjectPtr<UWidgetComponent> OverHeadWidgetComp;

	//检测头顶UI范围的定时器.
	FTimerHandle OverheadWidgetVisibilityTimer;

	//头顶UI显示检测的间隔.
	UPROPERTY(EditDefaultsOnly, Category = "UI | OverHead")
	float OverHeadWidgetVisibilityCheckGap = 1.f;

	//人物头顶UI显示范围. --- 这是使用的是实际距离的平方(10m) 这样更快.
	UPROPERTY(EditDefaultsOnly, Category = "UI | OverHead")
	float OverHeadWidgetVisibilityRange = 1000000.0f;

	void SetStatusGaugeEnabled(bool bEnabled);


	/*************************************************************
	 *					死亡与重生
	 *************************************************************/

	//死亡的蒙太奇动画.
	UPROPERTY(EditDefaultsOnly, Category = "Death and ReSpawn")
	TObjectPtr<UAnimMontage> DeathMontage;

	// 在死亡蒙太奇播放完毕 与 启用Ragdoll	之间的时间间隔.
	UPROPERTY(EditDefaultsOnly, Category = "Death and ReSpawn")
	float TimeShiftBeforeDeathMontage = 0.8f;

	//Mesh相当于Capsule的相对位置.
	FTransform MeshRelativeTransform;

	// 设置Ragdoll是否启用.
	void SetRagdollEnabled(bool bEnabled);

	void StartDeath();
	void ReSpawn();

	virtual void OnDeath();
	virtual void OnReSpawn();

public:
	/***************************************************
	 *           Team System
	 **************************************************/

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamId = NewTeamID; }

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	UFUNCTION()
	virtual void OnRep_TeamId();
	
private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_TeamId)
	FGenericTeamId TeamId;


	/***************************************************
	 *           AI System
	 **************************************************/

	// 启动/关闭AI感知组件的感知源.
	void SetAIPerceptionStimuliSourceEnabled(bool bEnabled);
	
	//AI感知组件, 添加了这个组建的Actor 才会被AI的感知系统感知到. --- 默认情况下会感知所有的Pawn 但是在DefaultGame.ini中关闭了该选项.
	UPROPERTY()
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSourceComp;
};
