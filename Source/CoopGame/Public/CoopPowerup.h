// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoopPickup.h"
#include "CoopPowerup.generated.h"

UCLASS()
class COOPGAME_API ACoopPowerup : public ACoopPickup
{
	GENERATED_BODY()

protected:
	/** Time between powerup */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	/** Total times we apply the powerup effect */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNrOfTicks;

	int32 TicksProcessed;

	FTimerHandle TimerHandle_PowerTick;
	
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
	void ActivatePowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

private:
	UFUNCTION()
	void OnTickPowerup();
};
