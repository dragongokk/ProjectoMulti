// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateProyectoPruebas.h"

#include "Net/UnrealNetwork.h"

void AGameStateProyectoPruebas::OnRep_ScoreBlue()
{
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(ScoreBlue));	
}

void AGameStateProyectoPruebas::OnRep_ScoreRed()
{
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(ScoreRed));	
}

void AGameStateProyectoPruebas::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameStateProyectoPruebas, ScoreBlue);
	DOREPLIFETIME(AGameStateProyectoPruebas, ScoreRed);
}
