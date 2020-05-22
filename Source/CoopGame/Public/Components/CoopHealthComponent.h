// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoopHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, class UCoopHealthComponent*, HealthComp,    float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent*, OwningHealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);


//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API UCoopHealthComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** Responsible for health */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultHealth;

	/** Responsible for health */
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;

public:	
	// Sets default values for this component's properties
	UCoopHealthComponent();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	/** Called when actor is damaged */
	UFUNCTION()
	void HandleAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	/** Custom Event */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	/** Delta health is added to current health if current health is bigger than 0.f */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float DeltaHealth);

	void SetCurrentHealth(float NewCurrentHealth);

private:
	UFUNCTION()
	void OnRep_CurrentHealth(float OldHealth);
};
