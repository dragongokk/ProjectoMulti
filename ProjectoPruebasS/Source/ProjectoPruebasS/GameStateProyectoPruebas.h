// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameStateProyectoPruebas.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API AGameStateProyectoPruebas : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(ReplicatedUsing = OnRep_ScoreBlue)
	int32 ScoreBlue;
	
	UPROPERTY(ReplicatedUsing = OnRep_ScoreRed)
	int32 ScoreRed;
	
	UFUNCTION()
	void OnRep_ScoreBlue();

	UFUNCTION()
	void OnRep_ScoreRed();
};
