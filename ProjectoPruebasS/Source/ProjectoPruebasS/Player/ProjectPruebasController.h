// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/PlayerController.h"
#include "ProjectPruebasController.generated.h"

class AProjectoPruebasSCharacter;
/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API AProjectPruebasController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	void LookUpAtRate(float X);

	void TurnAtRate(float X);

	void OnFire();

	void MoveForward(float X);

	void MoveInfiForward(float X);

	void MoveRight(float X);

	void Jump();
	
	void StopJumping();
	
	void AddControllerYawInput(float X);
	
	void AddControllerPitchInput(float X);
	
	virtual void SetupInputComponent() override;

	UPROPERTY()
	AProjectoPruebasSCharacter* MainCharacter;
	
};


