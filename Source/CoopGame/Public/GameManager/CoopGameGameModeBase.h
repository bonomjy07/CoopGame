// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CoopGameGameModeBase.generated.h"

// Custom event: Raised when actor who has health component dies
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

/**
 * 
 */
UCLASS()
class COOPGAME_API ACoopGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Customer event
	*   Called when actor who has health component dies */
	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWave;

	UPROPERTY(BlueprintReadOnly, Category = "GameMode")
	int32 NrOfPlayer;

public:
	ACoopGameGameModeBase();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void GameOver();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void Respawn();
};
