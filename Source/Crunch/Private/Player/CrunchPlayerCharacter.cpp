// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CrunchPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACrunchPlayerCharacter::ACrunchPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
}

void ACrunchPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* OwningPC = GetController<APlayerController>())
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = OwningPC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		EnhancedInputSubsystem->ClearAllMappings();
		EnhancedInputSubsystem->AddMappingContext(IMC_Gameplay, 0);
	}
}

void ACrunchPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ACrunchPlayerCharacter::Jump);
		EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ACrunchPlayerCharacter::HandleLook);
		EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ACrunchPlayerCharacter::HandleMove);

		for (auto Pair : GAToIAMap)
		{
			if (Pair.Value)
			{
				EnhancedInput->BindAction(Pair.Value, ETriggerEvent::Triggered, this, &ACrunchPlayerCharacter::HandleAbilityInput, Pair.Key);
			}
		}
	}
}

void ACrunchPlayerCharacter::HandleLook(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();

	AddControllerPitchInput(-InputVal.Y);
	AddControllerYawInput(InputVal.X);
}

void ACrunchPlayerCharacter::HandleMove(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	InputVal.Normalize();

	FVector WorldDirection = FVector::CrossProduct(ViewCamera->GetRightVector(), FVector::UpVector) * InputVal.Y
		+ ViewCamera->GetRightVector() * InputVal.X;

	AddMovementInput(WorldDirection);
}

void ACrunchPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ECrunchAbilityInputID AbilityInputID)
{
	if (bool bPressed = InputActionValue.Get<bool>())
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)AbilityInputID);
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)AbilityInputID);
	}
}

void ACrunchPlayerCharacter::OnDeath()
{
	if (APlayerController* OwningPC = GetController<APlayerController>())
	{
		DisableInput(OwningPC);
	}
}

void ACrunchPlayerCharacter::OnReSpawn()
{
	if (APlayerController* OwningPC = GetController<APlayerController>())
	{
		EnableInput(OwningPC);
	}
}
