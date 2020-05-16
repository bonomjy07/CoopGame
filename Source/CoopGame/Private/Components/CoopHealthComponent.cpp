// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CoopHealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCoopHealthComponent::UCoopHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Set default health
	DefaultHealth = 100.f;

	bReplicates = true;
}

void UCoopHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCoopHealthComponent, CurrentHealth);
}

// Called when the game starts
void UCoopHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only server can hook in cases of any error or bug
	//if (GetOwnerRole() == ROLE_Authority)
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

void UCoopHealthComponent::HandleAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	// Update health clamped
	CurrentHealth = FMath::Clamp<float>(CurrentHealth - Damage, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);

	UE_LOG(LogTemp, Log, TEXT("[%s] is damaged, Current Health is %s"), *DamagedActor->GetName() , *FString::SanitizeFloat(CurrentHealth));
}

void UCoopHealthComponent::OnRep_CurrentHealth(float old)
{
	float Damage = old - CurrentHealth;
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, nullptr, nullptr, nullptr);
}

