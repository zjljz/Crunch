// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CrunchPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Crunch/Crunch.h"
#include "Crunch/CrunchGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Crunch/Public/AbilitySystem/CrunchHeroAttributeSet.h"
#include "Inventory/InventoryComponent.h"

ACrunchPlayerCharacter::ACrunchPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->ProbeChannel = ECC_SpringArm;
	CameraBoom->bUsePawnControlRotation = true;

	HeroAttributeSet = CreateDefaultSubobject<UCrunchHeroAttributeSet>(TEXT("HeroAttributeSet"));

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
}

void ACrunchPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* OwningPC = GetController<APlayerController>())
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = OwningPC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		EnhancedInputSubsystem->RemoveMappingContext(IMC_Gameplay);
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
		EnhancedInput->BindAction(IA_UpgradeAbility, ETriggerEvent::Started, this, &ThisClass::HandleAbilityLeaderDown);
		EnhancedInput->BindAction(IA_UpgradeAbility, ETriggerEvent::Completed, this, &ThisClass::HandleAbilityLeaderUp);

		for (auto Pair : GAToIAMap)
		{
			if (Pair.Value)
			{
				EnhancedInput->BindAction(Pair.Value, ETriggerEvent::Triggered, this, &ACrunchPlayerCharacter::HandleAbilityInput, Pair.Key);
			}
		}
	}
}

void ACrunchPlayerCharacter::SetInputEnabled(bool bEnabled)
{
	if (APlayerController* OwningPC = GetController<APlayerController>())
	{
		if (bEnabled)
		{
			EnableInput(OwningPC);
		}
		else
		{
			DisableInput(OwningPC);
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

void ACrunchPlayerCharacter::HandleAbilityLeaderDown(const FInputActionValue& InputActionValue)
{
	bIsUpgradeAbilityKeyDown = true;
}

void ACrunchPlayerCharacter::HandleAbilityLeaderUp(const FInputActionValue& InputActionValue)
{
	bIsUpgradeAbilityKeyDown = false;
}

void ACrunchPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ECrunchAbilityInputID AbilityInputID)
{
	bool bPressed = InputActionValue.Get<bool>();
	if (bPressed && bIsUpgradeAbilityKeyDown)
	{
		UpgradeAbilityWithInputID(AbilityInputID);
	}

	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)AbilityInputID);
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)AbilityInputID);
	}

	if (AbilityInputID == ECrunchAbilityInputID::BasicAttack)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, CrunchGameplayTags::Ability_BasicAttack_Pressed, FGameplayEventData());
		Server_SendGameplayEventToSelf(CrunchGameplayTags::Ability_BasicAttack_Pressed, FGameplayEventData());
	}
}

void ACrunchPlayerCharacter::OnDeath()
{
	SetInputEnabled(false);
}

void ACrunchPlayerCharacter::OnReSpawn()
{
	SetInputEnabled(true);

	if (HasAuthority())
	{
		SetActorTransform(GetController()->StartSpot->GetActorTransform());
	}
}

void ACrunchPlayerCharacter::OnStartStun()
{
	SetInputEnabled(false);
}

void ACrunchPlayerCharacter::OnEndStun()
{
	SetInputEnabled(true);
}

void ACrunchPlayerCharacter::OnAimChanged(bool bIsAiming)
{
	if (IsLocallyControlled())
	{
		LerpCameraToLocalOffsetLocation(bIsAiming ? CameraAimLocalOffset : FVector::ZeroVector);
	}
}

void ACrunchPlayerCharacter::LerpCameraToLocalOffsetLocation(const FVector& TargetLoc)
{
	GetWorldTimerManager().ClearTimer(CameraLerpTimerHandle);
	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ACrunchPlayerCharacter::TickCameraLocalOffsetLerp, TargetLoc));
}

void ACrunchPlayerCharacter::TickCameraLocalOffsetLerp(FVector TargetLoc)
{
	FVector CurrentLoc = ViewCamera->GetRelativeLocation();
	if (FVector::Dist(CurrentLoc, TargetLoc) < 1.f)
	{
		ViewCamera->SetRelativeLocation(TargetLoc);
		return;
	}

	float LerpAlpha = FMath::Clamp(CameraLerpSpeed * GetWorld()->GetDeltaSeconds(), 0.f, 1.f);
	FVector NewLoc = FMath::Lerp(CurrentLoc, TargetLoc, LerpAlpha);
	ViewCamera->SetRelativeLocation(NewLoc);

	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ACrunchPlayerCharacter::TickCameraLocalOffsetLerp, TargetLoc));
}
