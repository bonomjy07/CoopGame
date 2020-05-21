// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopSpawnVolume.generated.h"

UCLASS()
class COOPGAME_API ACoopSpawnVolume : public AActor
{
	GENERATED_BODY()

	// The spawn area where pickups will be created
	UPROPERTY(VisibleAnywhere, Category = "Spawn", Meta = (AllowPrivatedAccess = "true"))
	class UBoxComponent* WhereToSpawn;

	// Acutial spawn delay
	float SpawnDelay;

protected:
	// This is the pickup to spawn
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> WhatToSpawn;

	// Timer for recurring spawning
	FTimerHandle SpawnTimer;

	// Minimun spawn deley
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Spawn")
	float SpawnRangeLow;

	// Maximum spawn deley
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Spawn")
	float SpawnRangeHigh;
	
public:	
	// Sets default values for this actor's properties
	ACoopSpawnVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SetSpawningActive(bool bShouldSpawn);

protected:
	// Find random point withn the box component
	UFUNCTION(BlueprintPure, Category = "Spawn")
	FVector GetRandomPointInVolume();
	
private:
	// Handles spawning a new pickup
	void SpawnActor();
};
