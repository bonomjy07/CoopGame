// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoopExplosiveActor.generated.h"

UCLASS()
class COOPGAME_API ACoopExplosiveActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCoopHealthComponent* HealthComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class URadialForceComponent* RadicalForceComponent;

protected:
	UPROPERTY(ReplicatedUsing=OnRep_IsExploded, BlueprintReadOnly, Category = "Explosion")
	bool bIsExploded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float DamageRadius;

	/** How much it'll blow up when it explodes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionImpulse;

	UPROPERTY(EditDefaultsOnly, Category = "vfx")
	class UMaterialInterface* ExplodedMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "vfx")
	class UParticleSystem* ExplosionEffect;

public:	
	// Sets default values for this actor's properties
	ACoopExplosiveActor();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnHealthChanged(class UCoopHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

private:
	UFUNCTION()
	void OnRep_IsExploded();
//	/** Change the material color when it is exploded */
//	UFUNCTION(BlueprintImplementableEvent)
//	void OnExplosion();
};
