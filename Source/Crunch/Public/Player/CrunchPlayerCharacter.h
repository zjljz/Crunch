// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CrunchCharacter.h"
#include "GAS/CrunchGameplayAbilityTypes.h"
#include "CrunchPlayerCharacter.generated.h"

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


private:
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	TObjectPtr<UCameraComponent> ViewCamera;

	
	/*****************************************
	 *				Input
	 *****************************************/


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Gameplay;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<ECrunchAbilityInputID, TObjectPtr<UInputAction>> GAToIAMap;

	void HandleLook(const FInputActionValue& InputActionValue);

	void HandleMove(const FInputActionValue& InputActionValue);

	void HandleAbilityInput(const FInputActionValue& InputActionValue, ECrunchAbilityInputID AbilityInputID);


	/********************************************************
	 *				Death and ReSpawn
	 *******************************************************/				

	virtual void OnDeath() override;

	virtual void OnReSpawn() override;
};
