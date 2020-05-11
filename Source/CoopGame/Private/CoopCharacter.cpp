// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopCharacter.h"
#include "Components/InputComponent.h"

// Sets default values
ACoopCharacter::ACoopCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACoopCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoopCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACoopCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement input
	PlayerInputComponent->BindAxis("MoveForward", this, &ACoopCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACoopCharacter::MoveRight);

	// Look input
	PlayerInputComponent->BindAxis("LookUp", this, &ACoopCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACoopCharacter::AddControllerYawInput);
}

void ACoopCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ACoopCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

