// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CoopHealthComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "CoopGameGameModeBase.h"

// Sets default values for this component's properties
UCoopHealthComponent::UCoopHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Set default health
	DefaultHealth = 100.f;

	// Set default dead state
	bIsDead = false;

	bReplicates = true;
}

void UCoopHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCoopHealthComponent, CurrentHealth);
	DOREPLIFETIME(UCoopHealthComponent, bIsDead);
}

// Called when the game starts
void UCoopHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only server can hook TakeAnyDamage delegator in cases of any error or bug
	if (GetOwnerRole() == ROLE_Authority)
	{
		// Register OnTakeAnyDamage handler
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UCoopHealthComponent::HandleAnyDamage);
		}
	}
	CurrentHealth = DefaultHealth;
}


// Called every frame
void UCoopHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Make sure this function is called by server only
void UCoopHealthComponent::HandleAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	// Update current health 
	SetCurrentHealth(FMath::Clamp<float>(CurrentHealth - Damage, 0.0f, DefaultHealth));
	// Update dead state
	bIsDead = (CurrentHealth <= 0.0f);

	// Raise HealthChanged event on server side
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);

	// Raise ActorKilled event if actor is dead
	if (bIsDead)
	{
		ACoopGameGameModeBase* GM = Cast<ACoopGameGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] is damaged, Damage Amount is %s, Current Health is %s"), *DamagedActor->GetName() , *FString::SanitizeFloat(Damage), *FString::SanitizeFloat(CurrentHealth));
}

void UCoopHealthComponent::Heal(float DeltaHealth)
{
	if (bIsDead)
	{
		return;
	}

	// Update current health
	SetCurrentHealth(FMath::Clamp<float>(CurrentHealth + DeltaHealth, 0.f, DefaultHealth));

	// Raise HealthChanged event on server side
	OnHealthChanged.Broadcast(this, CurrentHealth, DeltaHealth, nullptr, nullptr, nullptr);

	UE_LOG(LogTemp, Log, TEXT("[%s] is healed, Heal Amount is %s, Current Health is %s"), *GetOwner()->GetName(), *FString::SanitizeFloat(DeltaHealth), *FString::SanitizeFloat(CurrentHealth));
}

void UCoopHealthComponent::SetCurrentHealth(float NewCurrentHealth)
{
	AActor* MyOwner = GetOwner();
	if (MyOwner && MyOwner->Role == ROLE_Authority)
	{
		CurrentHealth = NewCurrentHealth;
	}
}

void UCoopHealthComponent::OnRep_CurrentHealth(float old)
{
	float Damage = old - CurrentHealth;
	// Raise HealthChanged event on client side
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, nullptr, nullptr, nullptr);
}

