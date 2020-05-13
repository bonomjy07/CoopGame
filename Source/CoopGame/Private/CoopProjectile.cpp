// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACoopProjectile::ACoopProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create sphere component to be root component and responsible for its collsion
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SphereComponent->SetSimulatePhysics(true);
	RootComponent = SphereComponent;

	// Create static mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMesh->SetupAttachment(RootComponent);

	// Create projectile movement component
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);

	ExplosionDelay = 2.0f;
	ExplosionDamage = 20.0f;
	ExplosionRadius = 100.f;
}

// Called when the game starts or when spawned
void ACoopProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(ExplosionDelay);
}

// Called every frame
void ACoopProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoopProjectile::Destroyed()
{
	Super::Destroyed();

	// Apply damage
	TArray<AActor*> IgnoreActors;
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, UDamageType::StaticClass(), IgnoreActors, this, GetInstigatorController());

	// Show explision effect
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());
}

