// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGameGameModeBase.h"
#include "CoopGameStateBase.h"
#include "CoopPlayerState.h"
#include "Engine/World.h"

ACoopGameGameModeBase::ACoopGameGameModeBase()
{
	// Set game state class
	GameStateClass = ACoopGameStateBase::StaticClass();

	// Set player state class
	PlayerStateClass = ACoopPlayerState::StaticClass();

	TimeBetweenWave = 10.f;

}

void ACoopGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void ACoopGameGameModeBase::GameOver()
{
}

void ACoopGameGameModeBase::Respawn()
{
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++)
	{
		APlayerController* PC =  it->Get();
		if (PC && !PC->GetPawn())
		{
			RestartPlayer(PC);
		}
	}
}

