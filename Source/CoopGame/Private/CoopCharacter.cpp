// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "AI/Navigation/NavigationTypes.h"

// Sets default values
ACoopCharacter::ACoopCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create camera boom for follow camera
	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoomComponent->bUsePawnControlRotation = true;
	CameraBoomComponent->TargetArmLength = 200.f;
	CameraBoomComponent->bEnableCameraLag = true;
	CameraBoomComponent->SetupAttachment(RootComponent);

	// Create camera component
	FollowCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCameraComponent"));
	FollowCameraComponent->SetupAttachment(CameraBoomComponent);

	// 'Crouch' needs 'Nav agent'
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;;
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
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACharacter::AddControllerYawInput);

	// Crouch input
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ACoopCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &ACoopCharacter::EndCrouch);

	// Jump input
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);
}

void ACoopCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ACoopCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ACoopCharacter::BeginCrouch()
{
	Crouch();
}

void ACoopCharacter::EndCrouch()
{
	UnCrouch();
}

