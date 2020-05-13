// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopProjectileWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ACoopProjectileWeapon::ACoopProjectileWeapon()
{
	MuzzleSocketName = "MuzzleFlashSocket";
}

void ACoopProjectileWeapon::Fire()
{
	UWorld* World = GetWorld();
	check(World);

	// Trace the world from pawn eye to crosshair location
	AActor* MyOwner = GetOwner();
	if (MyOwner && ProjectileClass)
	{
		// Get Eye transformation except scale
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		// Information for spawning projectile 
		FVector MuzzleLocation = WeaponMeshComponent->GetSocketLocation(MuzzleSocketName);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = GetInstigator();

		// Shoot projectile
		World->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
	}
}
