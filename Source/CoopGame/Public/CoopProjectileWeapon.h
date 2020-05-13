// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoopWeapon.h"
#include "CoopProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ACoopProjectileWeapon : public ACoopWeapon
{
	GENERATED_BODY()
	
public:
	ACoopProjectileWeapon();

public:
	// Fire projectile
	virtual void Fire() override;
	
	// Projectile to fire class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> ProjectileClass;
};
