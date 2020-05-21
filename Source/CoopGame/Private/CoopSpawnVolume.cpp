// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopSpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

// Sets default values
ACoopSpawnVolume::ACoopSpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if (Role == ROLE_Authority)
	{
		WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
		RootComponent = Cast<USceneComponent>(WhereToSpawn);

		// Sets some base values for range
		SpawnRangeLow = 1.f;
		SpawnRangeHigh = 3.f;
	}
}

// Called when the game starts or when spawned
void ACoopSpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority)
	{
		SpawnDelay = FMath::FRandRange(SpawnRangeLow, SpawnRangeHigh);
		SetSpawningActive(true);
	}
}

void ACoopSpawnVolume::SetSpawningActive(bool bShouldSpawn)
{
	if (bShouldSpawn)
	{
		SpawnDelay = FMath::FRandRange(SpawnRangeLow, SpawnRangeHigh);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ACoopSpawnVolume::SpawnActor, SpawnDelay, false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpawnTimer);
	}
}

FVector ACoopSpawnVolume::GetRandomPointInVolume()
{
	if (WhereToSpawn)
	{
		FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
		FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
		return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
	}
	return FVector();
}

void ACoopSpawnVolume::SpawnActor()
{
	if (Role == ROLE_Authority && WhatToSpawn)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			// Setup required params/info
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			// Finds random point to spawn
			FVector SpawnLocation = GetRandomPointInVolume();

			// Set a random rotation for the spawned pickup
			FRotator SpawnRotation;
			SpawnRotation.Yaw = FMath::FRand() * 360.f;
			SpawnRotation.Pitch = FMath::FRand() * 360.f;
			SpawnRotation.Roll = FMath::FRand() * 360.f;

			// Drops the new pickup into the world
			AActor* SpawnedActor = World->SpawnActor<AActor>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

			SetSpawningActive(true);
		}
	}
}

