// Fill out your copyright notice in the Description page of Project Settings.

#include "CoopCharacter.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CoopHealthComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Net/UnrealNetwork.h"

#include "CoopGame.h"
#include "CoopWeapon.h"

// Sets default values
ACoopCharacter::ACoopCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create camera boom for follow camera
	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoomComponent->bUsePawnControlRotation = true;
	CameraBoomComponent->TargetArmLength = 200.f;
	CameraBoomComponent->bEnableCameraLag = true;
	CameraBoomComponent->SetupAttachment(RootComponent);

	// Create camera component
	FollowCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCameraComponent"));
	FollowCameraComponent->SetupAttachment(CameraBoomComponent);

	// Create health component
	HealthComponent = CreateDefaultSubobject<UCoopHealthComponent>(TEXT("HealthCOmponent"));

	// Allow our custom collision to be ignored
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECollisionResponse::ECR_Ignore);
	
	// 'Crouch' needs 'Nav agent'
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;;

	// Set weapon attachment socket name
	WeaponAttachSocketName = "WeaponSocket";

	DefaultWeaponClass = ACoopWeapon::StaticClass();

	// Set default zoom values
	bZoomingIn = false;
	ZoomedFOV = 65.0f;
	ZoomSpeed = 0.2f;
}

void ACoopCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACoopCharacter, CurrentWeapon);
	DOREPLIFETIME(ACoopCharacter, bDied);
}

// Called when the game starts or when spawned
void ACoopCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set default FOV
	DefaultFOV = FollowCameraComponent->FieldOfView;

	// Create weapon and attch it to character
	if (Role == ROLE_Authority)
	{
		UWorld* World = GetWorld();
		check(World);
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CurrentWeapon = World->SpawnActor<ACoopWeapon>(DefaultWeaponClass, SpawnParameters);
		if (CurrentWeapon)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			CurrentWeapon->AttachToComponent(Cast<USceneComponent>(GetMesh()), AttachmentRules, WeaponAttachSocketName);
		}
	}

	// Register OnHealthChanged event
	HealthComponent->OnHealthChanged.AddDynamic(this, &ACoopCharacter::OnHealthChanged);
}

// Called every frame
void ACoopCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Zoom in if zoom button is pressed
	{
		if (bZoomingIn)
		{
			ZoomFactor += DeltaTime / ZoomSpeed;
		}
		else
		{
			ZoomFactor -= DeltaTime / ZoomSpeed;
		}
		ZoomFactor = FMath::Clamp<float>(ZoomFactor, 0.0f, 1.0f);
		FollowCameraComponent->FieldOfView = FMath::Lerp<float>(DefaultFOV, ZoomedFOV, ZoomFactor);
	}
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

	// Fire input
	//PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACoopCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACoopCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ACoopCharacter::StopFire);

	// Crouch input
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ACoopCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &ACoopCharacter::EndCrouch);

	// Jump input
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	// Zoom input
	PlayerInputComponent->BindAction("Zoom", EInputEvent::IE_Pressed, this, &ACoopCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", EInputEvent::IE_Released, this, &ACoopCharacter::EndZoom);
}

FVector ACoopCharacter::GetPawnViewLocation() const
{
	if (FollowCameraComponent)
	{
		return FollowCameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ACoopCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ACoopCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ACoopCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ACoopCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ACoopCharacter::BeginCrouch()
{
	Crouch();
}

void ACoopCharacter::EndCrouch()
{
	UnCrouch();
}

void ACoopCharacter::BeginZoom()
{
	bZoomingIn = true;
}

void ACoopCharacter::EndZoom()
{
	bZoomingIn = false;
}

void ACoopCharacter::OnHealthChanged(UCoopHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(3.f);
	}
}

