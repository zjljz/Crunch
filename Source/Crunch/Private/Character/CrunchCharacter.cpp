// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CrunchCharacter.h"

#include "Components/WidgetComponent.h"
#include "GAS/CrunchAbilitySystemComponent.h"
#include "GAS/CrunchAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/OverHeadStatsGauge.h"

ACrunchCharacter::ACrunchCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	CrunchASC = CreateDefaultSubobject<UCrunchAbilitySystemComponent>(TEXT("CrunchASC"));
	CrunchAttributeSet = CreateDefaultSubobject<UCrunchAttributeSet>(TEXT("CrunchAttributeSet"));

	OverHeadWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidgetComp"));
	OverHeadWidgetComp->SetupAttachment(RootComponent);
}

void ACrunchCharacter::BeginPlay()
{
	Super::BeginPlay();

	ConfigureOverHeadWidget();
}

void ACrunchCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACrunchCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!NewController->IsPlayerController())
	{
		ServerInit();
	}
}


UAbilitySystemComponent* ACrunchCharacter::GetAbilitySystemComponent() const
{
	return CrunchASC;
}

void ACrunchCharacter::ServerInit()
{
	CrunchASC->InitAbilityActorInfo(this, this);
	CrunchASC->ApplyInitialEffects();
	CrunchASC->GiveInitialAbilities();
}

void ACrunchCharacter::ClientInit()
{
	CrunchASC->InitAbilityActorInfo(this, this);
}

void ACrunchCharacter::ConfigureOverHeadWidget()
{
	if (!OverHeadWidgetComp)
	{
		return;
	}

	if (IsLocallyControlled())
	{
		OverHeadWidgetComp->SetHiddenInGame(true);
		return;
	}

	if (UOverHeadStatsGauge* OverHeadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComp->GetUserWidgetObject()))
	{
		OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());

		OverHeadWidgetComp->SetHiddenInGame(false);
		GetWorldTimerManager().ClearTimer(OverheadWidgetVisibilityTimer);
		GetWorldTimerManager().SetTimer(OverheadWidgetVisibilityTimer, this, &ACrunchCharacter::UpdateHeadGaugeVisibility,
		                                OverHeadWidgetVisibilityCheckGap, true);
	}
}

void ACrunchCharacter::UpdateHeadGaugeVisibility()
{
	if (APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		float DisToLocalPawn = FVector::DistSquared(GetActorLocation(), LocalPawn->GetActorLocation());
		OverHeadWidgetComp->SetHiddenInGame(DisToLocalPawn > OverHeadWidgetVisibilityRange);
	}
}
