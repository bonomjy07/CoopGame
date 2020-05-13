// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopProjectile.generated.h"

UCLASS()
class COOPGAME_API ACoopProjectile : public AActor
{
	GENERATED_BODY()

	// plz
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;

	// Projectile movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;

	// Projectile movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
protected:
	// Exploision effect when it hits something
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	UParticleSystem* ExplosionEffect;

public:
	// Life span to explode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionRadius;

public:	
	// Sets default values for this actor's properties
	ACoopProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Apply radical damage and show explosion effect
	virtual void Destroyed() override;
};
