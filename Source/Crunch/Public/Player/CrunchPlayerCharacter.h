// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CrunchCharacter.h"
#include "Crunch/Public/AbilitySystem/CrunchGameplayAbilityTypes.h"
#include "CrunchPlayerCharacter.generated.h"

class UInventoryComponent;
class UCrunchHeroAttributeSet;
struct FInputActionValue;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;

/**
 * 
 */
UCLASS()
class CRUNCH_API ACrunchPlayerCharacter : public ACrunchCharacter
{
	GENERATED_BODY()

public:
	ACrunchPlayerCharacter();

	virtual void PawnClientRestart() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	TObjectPtr<UCameraComponent> ViewCamera;

	/******************* InputSystem *******************/

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Gameplay;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_UpgradeAbility;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<ECrunchAbilityInputID, TObjectPtr<UInputAction>> GAToIAMap;

	bool bIsUpgradeAbilityKeyDown = false;

	//启用或禁用输入系统.
	void SetInputEnabled(bool bEnabled);

	void HandleLook(const FInputActionValue& InputActionValue);

	void HandleMove(const FInputActionValue& InputActionValue);

	void HandleAbilityLeaderDown(const FInputActionValue& InputActionValue);
	void HandleAbilityLeaderUp(const FInputActionValue& InputActionValue);

	void HandleAbilityInput(const FInputActionValue& InputActionValue, ECrunchAbilityInputID AbilityInputID);

	/*******************End InputSystem *******************/


public:
	virtual void OnDeath() override;
	virtual void OnReSpawn() override;

	virtual void OnStartStun() override;
	virtual void OnEndStun() override;


	/******************* Ability System *****************/

	virtual void OnAimChanged(bool bIsAiming) override;

private:
	UPROPERTY()
	TObjectPtr<UCrunchHeroAttributeSet> HeroAttributeSet;

	/***************** End Ability System *****************/


	/******************* CameraView *******************/

private:
	UPROPERTY(EditDefaultsOnly, Category = "CameraView")
	FVector CameraAimLocalOffset = FVector(0.f, 0.f, 50.f);

	UPROPERTY(EditDefaultsOnly, Category = "CameraView")
	float CameraLerpSpeed = 20.f;

	FTimerHandle CameraLerpTimerHandle;

	void LerpCameraToLocalOffsetLocation(const FVector& TargetLoc);
	void TickCameraLocalOffsetLerp(FVector TargetLoc);
	/******************* End CameraView *******************/


	/******************* Inventory System *******************/
	UPROPERTY(VisibleDefaultsOnly, Category = "Inventory System")
	TObjectPtr<UInventoryComponent> InventoryComp;

	/******************End Inventory System***********/
};
