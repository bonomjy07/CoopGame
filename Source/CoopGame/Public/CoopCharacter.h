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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	class ACoopWeapon* MyWeapon;

public:
	// Sets default values for this character's properties
	ACoopCharacter();

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

	// Called for crouch action start
	void BeginCrouch();

	// Called for crouch action end
	void EndCrouch();
};
