// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialFunctionInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/DamageType.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"

#include "Components/CoopHealthComponent.h"
#include "CoopCharacter.h"

// Sets default values
ACoopTrackerBot::ACoopTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create static mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetCanEverAffectNavigation(false);
	StaticMeshComponent->SetSimulatePhysics(true);
	RootComponent = StaticMeshComponent;

	// Create sphere component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(200.0);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComponent->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UCoopHealthComponent>(TEXT("HealthComponent"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	ExplosionDamage = 30.f;
	ExplosionRadius = 100.f;

	SelfDamageInterpol = 0.5f;

	bReplicateMovement = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACoopTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnHealthChanged.AddDynamic(this, &ACoopTrackerBot::HandleTakeDamage);

	if (Role == ROLE_Authority)
	{
		// ...
	}
}

// Called every frame
void ACoopTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && !bIsExploded)
	{
		NextPathPoint = GetNextPathPoint();
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		StaticMeshComponent->AddForce(ForceDirection, NAME_None, true);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 2.f);
	}
}

void ACoopTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartedSelfDestruction && !bIsExploded)
	{
		ACoopCharacter* PlayerPawn = Cast<ACoopCharacter>(OtherActor);
		if (PlayerPawn)
		{
			if (Role == ROLE_Authority)
			{
				// Start self destruction sequnces.
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ACoopTrackerBot::SelfDamage, SelfDamageInterpol, true, 0.0f);
			}

			bStartedSelfDestruction = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructionSound, RootComponent);
		}
	}

}

FVector ACoopTrackerBot::GetNextPathPoint()
{
	UWorld* World = GetWorld();
	check(World);

	// Nearest player pawn is the target
	APawn* Target = nullptr;
	float NearestTargetDistance = FLT_MAX;
	for (FConstPawnIterator It = World->GetPawnIterator(); It; ++It)
	{
		APawn* Pawn = It->Get();
		if (Pawn && Pawn->IsControlled())
		{
			float DistanceBetweenTargetAndMe = (Pawn->GetActorLocation() - GetActorLocation()).Size();
			if (DistanceBetweenTargetAndMe < NearestTargetDistance)
			{
				NearestTargetDistance = DistanceBetweenTargetAndMe;
				Target = Pawn;
			}
		}
	}

	// Track the player
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), Target);

	// Return new path
	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ACoopTrackerBot::SelfDestruct()
{
	if (bIsExploded)
	{
		return;
	}

	bIsExploded = true;

	// Show explosion particle
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());

	// Play explosion sound
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

	// Fake it as disappear
	StaticMeshComponent->SetVisibility(false);
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AudioComponent->Stop();

	if (Role == ROLE_Authority)
	{
		// Apply the damage
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Push(this);
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, UDamageType::StaticClass(), IgnoreActors, this, GetInstigatorController(), false);

		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 8, FColor::Red, false, 2, 0, 1);

		//Destroy();
		SetLifeSpan(2.f);
	}
}

void ACoopTrackerBot::SelfDamage()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ACoopTrackerBot::HandleTakeDamage(UCoopHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInst == nullptr)
	{
		MatInst = StaticMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, StaticMeshComponent->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0.0f)
	{
		SelfDestruct();
	}
}

void ACoopTrackerBot::OnCheckNearbyBots()
{
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(600.0f);

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	QueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);

	TArray<FOverlapResult> Overlaps;

	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollisionShape);

	int32 NrOfBots = 0;
	for (FOverlapResult Result : Overlaps)
	{
		ACoopTrackerBot* Bot = Cast<ACoopTrackerBot>(Result.GetActor());
		if (Bot && Bot != this)
		{
			NrOfBots++;
		}
	}

	const int32 MaxPowerLevel = 4;

	PowerLevel = FMath::Clamp(NrOfBots, 0, MaxPowerLevel);

	if (MatInst == nullptr)
	{
		MatInst = StaticMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, StaticMeshComponent->GetMaterial(0));
	}
	if (MatInst)
	{
		float Alpha = PowerLevel / (float)MaxPowerLevel;

		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}
}
