// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CoopGameStateBase.generated.h"

UENUM(BlueprintType)
enum ECoopGameState
{
	EWaitingToStart, 

	EWaveinProgress,

	EWaitingToComplemete,

	EWaveComplete,

	EGameOver
};

/**
 * 
 */
UCLASS()
class COOPGAME_API ACoopGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	/** Current game state */
	UPROPERTY(ReplicatedUsing=OnRep_CurrentGameState, EditDefaultsOnly, Category = "GameState")
	TEnumAsByte<enum ECoopGameState> CurrentGameState;

public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	enum ECoopGameState GetCurrentGameState() const;

	void SetCurrentGameState(enum ECoopGameState NewGameState);

protected:
	UFUNCTION()
	void OnRep_CurrentGameState(enum ECoopGameState OldGameState);
};
