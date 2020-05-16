// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoopCharacter.generated.h"

UCLASS()
class COOPGAME_API ACoopCharacter : public ACharacter
{
	GENERATED_BODY()

	// Camera boom positioning the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoomComponent;

	// Third-Person Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCameraComponent;

	// Health component responsible for taking damage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCoopHealthComponent* HealthComponent;

protected:
	// Character is currenlty holding weapon
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Weapon")
	class ACoopWeapon* CurrentWeapon;

	// Default weapon class character is holding at begin
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class ACoopWeapon> DefaultWeaponClass;

	// Socket name where to attach the weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	// true if zoom button is pressed else, false
	float bZoomingIn;

	// Store default FOV when game begins
	float DefaultFOV;

	// Increments when zoom button is pressed else, decrements
	float ZoomFactor;

	// How much character zoom in
	UPROPERTY(EditDefaultsOnly, Category = "Gunplay")
	float ZoomedFOV;

	// How fast character zoom in
	UPROPERTY(EditDefaultsOnly, Category = "Gunplay")
	float ZoomSpeed;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterState")
	bool bDied;

public:
	// Sets default values for this character's properties
	ACoopCharacter();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Returns Follow Camera location
	virtual FVector GetPawnViewLocation() const override;

protected:
	// Called for forward/backward input
	void MoveForward(float Value);

	// Called for right/left input
	void MoveRight(float Value);

	// Called when fire button is pressed
	void StartFire();

	// Called when fire button is pressed
	void StopFire();

	// Called when crouch button is pressed
	void BeginCrouch();

	// Called when crouch button is released
	void EndCrouch();

	// Called when zoom button is pressed
	void BeginZoom();

	// Called when zoom button is released
	void EndZoom();

protected:
	UFUNCTION()
	void OnHealthChanged(class UCoopHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
