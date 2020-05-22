// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopExplosiveActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CoopHealthComponent.h"
#include "GameFramework/DamageType.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACoopExplosiveActor::ACoopExplosiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create static mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	RootComponent = StaticMeshComponent;

	// Create health component
	HealthComponent = CreateDefaultSubobject<UCoopHealthComponent>(TEXT("HealthComponent"));

	// Create radical force component
	RadicalForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadicalForceComponent"));
	RadicalForceComponent->SetupAttachment(StaticMeshComponent);
	RadicalForceComponent->bIgnoreOwningActor = true;
	RadicalForceComponent->bAutoActivate = false;
	RadicalForceComponent->bImpulseVelChange = true;
	RadicalForceComponent->Radius = 250.f;

	// Set default explosion variables
	bIsExploded = false;

	// Set default damage varaibles
	BaseDamage = 20.f;
	DamageRadius = 250.f;

	bReplicateMovement = true;
	bReplicates = true;
}

void ACoopExplosiveActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACoopExplosiveActor, bIsExploded);
}

// Called when the game starts or when spawned
void ACoopExplosiveActor::BeginPlay()
{
	Super::BeginPlay();

	// Set default impulse
	ExplosionImpulse = StaticMeshComponent->GetMass() * 1000.f;
	
	if (Role == ROLE_Authority)
	{
		HealthComponent->OnHealthChanged.AddDynamic(this, &ACoopExplosiveActor::OnHealthChanged);
	}
}

// Called every frame
void ACoopExplosiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoopExplosiveActor::OnHealthChanged(UCoopHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bIsExploded)
	{
		// I'm dead
		bIsExploded = true;

		OnRep_IsExploded(); // Fake call for function

		// Blow up
		FVector BlowUpImpulse = GetActorUpVector() * ExplosionImpulse;
		StaticMeshComponent->AddImpulse(BlowUpImpulse);

		// Apply damage
		TArray<AActor*> IgnoreActors; // None
		IgnoreActors.Push(this);
		UGameplayStatics::ApplyRadialDamage(this, BaseDamage, GetActorLocation(), DamageRadius, UDamageType::StaticClass(), IgnoreActors, DamageCauser, InstigatedBy);

		// Blast away nearby physics actors
		RadicalForceComponent->FireImpulse();

		DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 8, FColor::Yellow, false, 5, 0, 3);
	}
}

void ACoopExplosiveActor::OnRep_IsExploded()
{
	// Change material explodedMaterial
	StaticMeshComponent->SetMaterial(0, ExplodedMaterial);

	// Show vfx
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation());
}
