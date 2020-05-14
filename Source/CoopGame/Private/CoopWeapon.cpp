// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ACoopWeapon::ACoopWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create skeleton mesh component
	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComp"));
	RootComponent = WeaponMeshComponent;

	// Set muzzle socket name
	MuzzleSocketName = "MuzzleSocket";
	TraceTargetName = "BeamEnd";
}

// Called when the game starts or when spawned
void ACoopWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoopWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoopWeapon::Fire()
{
	UWorld* World = GetWorld();
	check(World);

	// Trache the world from pawn eye to crosshair location
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		// Get Eye transformation except scale
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		// Ready to shoot
		FHitResult HitResult;
		FVector EndTrace = EyeLocation + EyeRotation.Vector()* 1000.0f;
		FVector TraceEndPoint = EndTrace;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true; // more precisely
		
		// Something was hit
		if (World->LineTraceSingleByChannel(HitResult, EyeLocation, EndTrace, ECollisionChannel::ECC_Visibility, QueryParams))
		{
			// Blocking was hit!
			UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), 20.f, EyeRotation.Vector(), HitResult, MyOwner->GetInstigatorController(), this, DamageType);
			// Show impact effect when something was hit
			UGameplayStatics::SpawnEmitterAtLocation(World, ImpactEffect, HitResult.Location, HitResult.ImpactNormal.Rotation());

			TraceEndPoint = HitResult.ImpactPoint;
		}

		PlayFireEffect(TraceEndPoint);

		if (DebugWeaponDrawing)
		{
			DrawDebugLine(World, EyeLocation, EndTrace, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	}
}

void ACoopWeapon::PlayFireEffect(const FVector& TraceEndPoint)
{
	// Show muzzle effect when it fires
	UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, WeaponMeshComponent, MuzzleSocketName);

	// Show trace effect when it fires
	FVector MuzzleLocation = WeaponMeshComponent->GetSocketLocation(MuzzleSocketName);
	UParticleSystemComponent* TraceEffectComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);
	if (TraceEffectComp)
	{
		TraceEffectComp->SetVectorParameter(TraceTargetName, TraceEndPoint);
	}
}
