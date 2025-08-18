// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CrunchAnimInstanceBase.generated.h"

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class CRUNCH_API UCrunchAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	// the below functions are the native overrides for each phase
	// Native initialization override point
	virtual void NativeInitializeAnimation() override;

	// Native update override point. It is usually a good idea to simply gather data in this step and 
	// for the bulk of the work to be done in NativeThreadSafeUpdateAnimation.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Native thread safe update override point. Executed on a worker thread just prior to graph update 
	// for linked anim instances, only called when the hosting node(s) are relevant
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

public:
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed > 0.f; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsJumping() const { return bIsJumping; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawSpeed() const { return YawSpeed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetFwdSpeed() const { return FwdSpeed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetRightSpeed() const { return RightSpeed; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSmoothedYawSpeed() const { return SmoothedYawSpeed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookYawOffset() const { return LookRotOffset.Yaw; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookPitchOffset() const { return LookRotOffset.Pitch; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	bool bShouldDoFullBody() const;
	
private:
	//动画实例的拥有者角色
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	//动画实例的拥有者角色的移动组件
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerMovementComp;

	//是否正在跳跃
	bool bIsJumping = false;

	//Character 的移动速度
	float Speed;

	//角色的Yaw速度, 即左右转动的速度
	float YawSpeed;

	// 上一帧的角色旋转 用来计算YawSpeed
	FRotator BodyLastRot;

	// 平滑插值的Yaw速度
	float SmoothedYawSpeed = 0.0f;

	bool bIsAiming = false;

	float RightSpeed = 0.0f;
	float FwdSpeed = 0.0f;
	
	// 平滑插值速度 用于平滑YawSpeed
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float SmoothLerpSpeedForYawSpeed = 1.0f;

	// 平滑插值速度 用于平滑YawSpeed
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float YawSpeedLerpToZeroSpeed = 30.0f;
	
	// 角色的视角偏移 用来混合AimOffset动画 控制人物头的转向
	FRotator LookRotOffset;
};
