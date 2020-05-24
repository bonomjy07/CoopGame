// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGameStateBase.h"
#include "Net/UnrealNetwork.h"

void ACoopGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACoopGameStateBase, CurrentGameState);
}

ECoopGameState ACoopGameStateBase::GetCurrentGameState() const
{
	return CurrentGameState;
}

void ACoopGameStateBase::SetCurrentGameState(ECoopGameState NewGameState)
{
	if (Role == ROLE_Authority)
	{
		CurrentGameState = NewGameState;
	}
}

void ACoopGameStateBase::OnRep_CurrentGameState(ECoopGameState OldGameState)
{
}
