// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopPowerup.generated.h"

UCLASS()
class COOPGAME_API ACoopPowerup : public AActor
{
	GENERATED_BODY()

	/** Powerup mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;

	/** Rotating movement */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class URotatingMovementComponent* RotatingMovementComponent;

	/** Time between powerup effect is applied */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	/** Total times we apply the powerup effect */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNrOfIntervals;

	/** Total number of interval applied */
	int32 IntervalProcessed;

	FTimerHandle TimerHandle_PowerupTick;

public:	
	// Sets default values for this actor's properties
	ACoopPowerup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void ActivatePowerup(AActor* Activator);

	// Called when powerup is activated
	UFUNCTION(BlueprintNativeEvent, Category = "Powerups")
	void OnActivated(AActor* Activator);
	virtual void OnActivated_Implementation(AActor* Activator);

	// Called per powerup interval
	UFUNCTION(BlueprintNativeEvent, Category = "Powerups")
	void OnPowerupInterval();
	virtual void OnPowerupInterval_Implementation();

	// Called when tick processed is bigger than total number of ticks
	UFUNCTION(BlueprintNativeEvent, Category = "Powerups")
	void OnExpired();
	virtual void OnExpired_Implementation();

	// Play particle effects
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void WasActivated(AActor* Activator); 

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Powerups")
	void ClientActivatedByPawn(AActor* Activator);
	virtual void ClientActivatedByPawn_Implementation(AActor* Activator);
};
