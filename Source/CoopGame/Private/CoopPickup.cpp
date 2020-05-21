// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopPickup.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
ACoopPickup::ACoopPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(75.f);
	RootComponent = SphereComponent;
	
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComponent->DecalSize = FVector(64, 75, 75);
	DecalComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACoopPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoopPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACoopPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

