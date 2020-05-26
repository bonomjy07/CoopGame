// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
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

	// Replication setting
	bReplicates = true;
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void ACoopWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ACoopWeapon, HitScanTrace, COND_SkipOwner); // vfx is already played in server side
	DOREPLIFETIME(ACoopWeapon, HitScanTrace);
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
	if (Role < ROLE_Authority)
	{
		ServerFire();
		return;
	}

	// Trache the world from pawn eye to crosshair location
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		UWorld* World = GetWorld();
		check(World);

		// Get Eye transformation except scale
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		// Where trace ends
		FVector EndTrace = EyeLocation + EyeRotation.Vector() * 2000.0f;
		FVector TraceEndPoint = EndTrace;

		// Collision setting
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;           // more precisely
		QueryParams.bReturnPhysicalMaterial = true; // give me surface type

		// Something was hit
		FHitResult HitResult;
		EPhysicalSurface SurfaceType = EPhysicalSurface::SurfaceType_Default;
		if (World->LineTraceSingleByChannel(HitResult, EyeLocation, EndTrace, COLLISION_WEAPON, QueryParams))
		{
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

			// Apply the damage
			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 3.0f;
			}
			UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), ActualDamage, EyeRotation.Vector(), HitResult, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			PlayImpactEffect(SurfaceType, HitResult.ImpactPoint);

			// Set where line trace ended
			TraceEndPoint = HitResult.ImpactPoint;
		}

		// Store last fired time to fix shooting bug 
		LastFiredTime = World->GetTimeSeconds();

		// Show some effects for gunshot
		PlayFireEffect(TraceEndPoint);
		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TraceEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		if (DebugWeaponDrawing)
		{
			DrawDebugLine(World, EyeLocation, EndTrace, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	}
}

void ACoopWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ACoopWeapon::ServerFire_Validate()
{
	return true;
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

void ACoopWeapon::OnRep_HitScanTrace()
{
	PlayFireEffect(HitScanTrace.TraceTo);

	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
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

void ACoopWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, const FVector& ImpactPoint)
{
	// Set impact effect depends on surface type
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

	// Set direction to spawn particle
	FVector MuzzleLocation = WeaponMeshComponent->GetSocketLocation(MuzzleSocketName);
	FVector ShotDirection = ImpactPoint - MuzzleLocation;
	ShotDirection.Normalize();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
}
