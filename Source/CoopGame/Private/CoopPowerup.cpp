// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopPowerup.h"
#include "TimerManager.h"

ACoopPowerup::ACoopPowerup()
{
	PowerupInterval = 0.0f;

	TotalNrOfTicks = 0;

}

void ACoopPowerup::BeginPlay()
{
	Super::BeginPlay();

	ActivatePowerup();
}

void ACoopPowerup::ActivatePowerup()
{
	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerTick, this, &ACoopPowerup::OnTickPowerup, PowerupInterval, true, 0.0f);
	}
	else
	{
		OnTickPowerup();
	}
}

void ACoopPowerup::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerTicked();

	if (TicksProcessed >= TotalNrOfTicks)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerTick);
	}
}
