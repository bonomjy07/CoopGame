// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopWeapon.generated.h"

/** Contains information of single hitscan weapon*/
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class COOPGAME_API ACoopWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoopWeapon();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

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

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();
	void ServerFire_Implementation();
	bool ServerFire_Validate();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopFire();

	// Damage type of the weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	// Camera Shake when it fires
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UCameraShake> ShakeClass;

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

	// Base damage for this weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage;

	// Spawned when it fires
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* MuzzleEffect;

	// Spawned when it fires
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* TraceEffect;

	// Spawned when it hits successfully
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* DefaultImpackEffect;

	// Spawned when it hits something flesh successfully
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* FleshImpactEffect;

protected:
	/** Used to timer the weapon firing */
	FTimerHandle Timer_Firing;

	// RPM : Bullets per minute for weapon firing
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	/** Time between bullets */
	float TimeBetweenShots;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

protected:
	// Called when weapon is fired
	void PlayFireEffect(const FVector& TraceEndPoint);

	void PlayImpactEffect(EPhysicalSurface SurfaceType, const FVector& ImpactPoint);

private:
	// Helper var to store last fired time to fix shooting bug
	float LastFiredTime;
};
