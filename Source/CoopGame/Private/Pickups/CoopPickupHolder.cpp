// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopPickupHolder.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TimerManager.h"

#include "CoopPowerup.h"

// Sets default values
ACoopPickupHolder::ACoopPickupHolder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create sphere component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(75.f);
	RootComponent = SphereComponent;

	// Create decal component
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComponent->DecalSize = FVector(64, 75, 75);
	DecalComponent->SetupAttachment(RootComponent);

	CooldownDuration = 5.f;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ACoopPickupHolder::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		Respawn();
	}
}

// Called every frame
void ACoopPickupHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoopPickupHolder::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Only server respawns the powerup 
	if (Role == ROLE_Authority)
	{
		if (SpawnedPowerup)
		{
			SpawnedPowerup->ActivatePowerup(OtherActor);
			SpawnedPowerup = nullptr;

			GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ACoopPickupHolder::Respawn, CooldownDuration);
		}
	}
}

void ACoopPickupHolder::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUpClass is nullptr. plz update PowerUpclass in Blueprint"));
		return;
	}

	UWorld* World = GetWorld();
	check(World);

	// Respawn the powerup
	FVector Location = GetActorLocation() + FVector(0, 0, 50.f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnedPowerup = World->SpawnActor<ACoopPowerup>(PowerUpClass, Location, FRotator::ZeroRotator,SpawnParams);
}

