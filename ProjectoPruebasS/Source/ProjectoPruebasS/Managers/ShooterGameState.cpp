// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameState.h"

#include "Net/UnrealNetwork.h"

void AShooterGameState::OnRep_ScoreBlue()
{
	/*
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(ScoreBlue));
		*/
}

void AShooterGameState::OnRep_ScoreRed()
{
	/*
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(ScoreRed));
		*/
}

void AShooterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShooterGameState, ScoreBlue);
	DOREPLIFETIME(AShooterGameState, ScoreRed);
}
