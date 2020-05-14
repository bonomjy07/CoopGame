// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopWeapon.generated.h"

UCLASS()
class COOPGAME_API ACoopWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoopWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Handle when it fire a weapon
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();

	// Damage type of the weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

protected:
	// Gun skeleton mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class USkeletalMeshComponent* WeaponMeshComponent;

	// Socket name for where to spawn an emitter on weapon mesh component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	// Parameter name for target in trace effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TraceTargetName;

	// Spawned when it fires
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* MuzzleEffect;

	// Spawned when it fires
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* TraceEffect;

	// Spawned when it hits something successfully
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* ImpactEffect;

	// Called when weapon is fired
	void PlayFireEffect(const FVector& TraceEndPoint);
};
