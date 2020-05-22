// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopPickupHolder.generated.h"

UCLASS()
class COOPGAME_API ACoopPickupHolder : public AActor
{
	GENERATED_BODY()

	// Sphere component to be overlapped when pawn is entered
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	// Represents circle
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* DecalComponent;

	// Powerup class to hold
	UPROPERTY(EditInstanceOnly, Category = "PickupHolder")
	TSubclassOf<class ACoopPowerup> PowerUpClass;
	class ACoopPowerup* SpawnedPowerup;

	// Delay to respawn the powerup
	UPROPERTY(EditInstanceOnly, Category = "PickupHolder")
	float CooldownDuration;

	FTimerHandle TimerHandle_RespawnTimer;

public:	
	// Sets default values for this actor's properties
	ACoopPickupHolder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	void Respawn();
};
