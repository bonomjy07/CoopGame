// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopPowerup.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACoopPowerup::ACoopPowerup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create static mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = StaticMeshComponent;

	// Create rotating movement component
	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));

	// Set default values
	PowerupInterval = 0.5f;
	TotalNrOfIntervals = 5;
	IntervalProcessed = 0;

	// Set replicates
	bReplicates = true;
	bReplicateMovement = true;
}

// Called when the game starts or when spawned
void ACoopPowerup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoopPowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoopPowerup::ActivatePowerup(AActor* Activator)
{
	if (Role == ROLE_Authority)
	{
		OnActivated(Activator);

		if (PowerupInterval > 0.f)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ACoopPowerup::OnPowerupInterval, PowerupInterval, true);
		}
		else
		{
			OnPowerupInterval();
		}
	}
}

void ACoopPowerup::OnActivated_Implementation(AActor* Activator)
{
	// Fake it like disappear
	SetActorHiddenInGame(true);
}

void ACoopPowerup::OnPowerupInterval_Implementation()
{
	// Time to be expired
	if (IntervalProcessed >= TotalNrOfIntervals)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
	++IntervalProcessed;
}

void ACoopPowerup::OnExpired_Implementation()
{
	Destroy();
}

void ACoopPowerup::ClientActivatedByPawn_Implementation(AActor* Activator)
{
	WasActivated(Activator);
}

