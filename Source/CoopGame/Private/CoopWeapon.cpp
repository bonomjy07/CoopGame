// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

#include "CoopGame.h"

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

	// Set base damage
	BaseDamage = 20.0f;

	// Bullets per min
	RateOfFire = 600.0f;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ACoopWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60.0f / RateOfFire;
	
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
		QueryParams.bTraceComplex = true;           // more precisely
		QueryParams.bReturnPhysicalMaterial = true; // give me surface type
		
		// Something was hit
		if (World->LineTraceSingleByChannel(HitResult, EyeLocation, EndTrace, COLLISION_WEAPON, QueryParams))
		{
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

			// Apply the damage
			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 3.0f;
			}
			UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), ActualDamage, EyeRotation.Vector(), HitResult, MyOwner->GetInstigatorController(), this, DamageType);

			// Show impact effect depends on surface
			UParticleSystem* SelectedEffect = nullptr;
			switch (SurfaceType)
			{
				case SURFACE_FLESHDEFAULT:
				case SURFACE_FLESHVULNERABLE:
					SelectedEffect = FleshImpactEffect;
					break;
				default:
					SelectedEffect = DefaultImpackEffect;
					break;
			}
			UGameplayStatics::SpawnEmitterAtLocation(World, SelectedEffect, HitResult.Location, HitResult.ImpactNormal.Rotation());

			// Set where line trace ended
			TraceEndPoint = HitResult.ImpactPoint;

			if (HitResult.GetComponent()->IsSimulatingPhysics())
			{
				FVector Velocity = HitResult.ImpactNormal * -1.f * 2000.f;
				HitResult.GetComponent()->AddImpulse(Velocity);
			}
		}

		// Show some effects for gunshot
		PlayFireEffect(TraceEndPoint);

		// Store last fired time to fix shooting bug 
		LastFiredTime = World->GetTimeSeconds();

		if (DebugWeaponDrawing)
		{
			DrawDebugLine(World, EyeLocation, EndTrace, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	}
}

void ACoopWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->GetTimeSeconds(), 0.f);
	GetWorldTimerManager().SetTimer(Timer_Firing, this, &ACoopWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ACoopWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(Timer_Firing);
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

	// Shake camera
	APawn* MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn)
	{
		APlayerController* PC = Cast<APlayerController>(MyPawn->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(ShakeClass);
		}
	}
}
