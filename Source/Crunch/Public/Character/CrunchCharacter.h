// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "GAS/CrunchAbilitySystemComponent.h"
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

	// 当Character被Controller控制时调用 --- 这个函数只在Server上调用.
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;


	/***************** Ability System *****************/
private:
	UPROPERTY(VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UCrunchAbilitySystemComponent> CrunchASC;

	UPROPERTY(VisibleAnywhere, Category = "Ability System")
	TObjectPtr<UCrunchAttributeSet> CrunchAttributeSet;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendGameplayEventToSelf(FGameplayTag EventTag, const FGameplayEventData& Payload);

	bool Server_SendGameplayEventToSelf_Validate(FGameplayTag EventTag, const FGameplayEventData& Payload) { return true; }
	const TMap<ECrunchAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return CrunchASC->GetAbilities(); }
	
	void ServerInit();
	void ClientInit();

	//绑定GAS相关的Tag变化事件. @todo: 更新函数名以及说明.
	void BindAbilitySystemTagChangeDelegate();

	void OnAttributeMoveSpeedUpdate(const FOnAttributeChangeData& Data);
	
	//当AimTag的数量发生变化时调用.
	void OnAimTagUpdate(const FGameplayTag Tag, int32 NewCount);
	
	//当DeadTag的数量发生变化时调用.
	void OnDeadTagUpDate(const FGameplayTag Tag, int32 NewCount);

	//当StunTag的数量发生变化时调用.
	void OnStunTagUpdate(const FGameplayTag Tag, int32 NewCount);
	
	//检查身上是否有dead的Tag来判断是否死亡.
	bool IsDead() const;

	//移除身上的Dead Tag.
	void RemoveDeadTag();

	void SetIsAiming(bool bIsAiming);

	virtual void OnAimChanged(bool bIsAiming) {};
	
	/**************** End Ability System *****************/


	/********************* OverHeadWidget *********************/
private:
	//头顶UI组件.
	UPROPERTY(VisibleAnywhere, Category = "UI | OverHead")
	TObjectPtr<UWidgetComponent> OverHeadWidgetComp;

	//头顶UI显示检测的间隔.
	UPROPERTY(EditDefaultsOnly, Category = "UI | OverHead")
	float OverHeadWidgetVisibilityCheckGap = 1.f;

	//人物头顶UI显示范围. --- 这是使用的是实际距离的平方(10m) 这样更快.
	UPROPERTY(EditDefaultsOnly, Category = "UI | OverHead")
	float OverHeadWidgetVisibilityRange = 1000000.0f;

	//检测头顶UI范围的定时器.
	FTimerHandle OverheadWidgetVisibilityTimer;

public:
	//为OverHeadUI 进行初始化.
	void ConfigureOverHeadWidget();

	//定时检测是否需要显示头顶UI.
	void UpdateHeadGaugeVisibility();

	//设置头顶UI是否启用.
	void SetStatusGaugeEnabled(bool bEnabled);

	/****************** End OverHeadWidget ******************/


private:
	/****************** Death and ReSpawn ******************/

	//Death 的 MontageAnim.
	UPROPERTY(EditDefaultsOnly, Category = "Death and ReSpawn")
	TObjectPtr<UAnimMontage> DeathMontage;

	// 在死亡蒙太奇播放完毕 与 启用Ragdoll	之间的时间间隔.
	UPROPERTY(EditDefaultsOnly, Category = "Death and ReSpawn")
	float TimeShiftBeforeDeathMontage = 0.8f;

	//Mesh相当于Capsule的相对位置.
	FTransform MeshRelativeTransform;

	// 设置Ragdoll是否启用.
	void SetRagdollEnabled(bool bEnabled);

public:


	// 这俩个函数是 Character 和 AI 的通用 Death & ReSpawn 逻辑.
	void StartDeath();
	void ReSpawn();

	// 这里是让子类覆盖的 Death & ReSpawn 逻辑.
	virtual void OnDeath()
	{
	};

	virtual void OnReSpawn()
	{
	};

	/*************** End Death and ReSpawn ******************/


	/********************** Stun **********************/
private:
	//眩晕状态的Montage.
	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	TObjectPtr<UAnimMontage> StunMontage;

public:
	
	//分别是开始眩晕和结束眩晕时 子类该覆盖的函数.
	virtual void OnStartStun()
	{
	};

	virtual void OnEndStun()
	{
	};
	/*******************End Stun *********************/


public:
	/***************************************************
	 *           Team System
	 **************************************************/

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamId = NewTeamID; }

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	UFUNCTION()
	virtual void OnRep_TeamId();

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_TeamId)
	FGenericTeamId TeamId;

private:
	/***************************************************
	 *           AI System
	 **************************************************/


	//AI感知组件, 添加了这个组件的Actor 才会被AI的感知系统感知到. --- 默认情况下会感知所有的Pawn 但是在DefaultGame.ini中关闭了该选项.
	UPROPERTY()
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSourceComp;

	// 启动/关闭AI感知组件的感知源.
	void SetAIPerceptionStimuliSourceEnabled(bool bEnabled);
};
