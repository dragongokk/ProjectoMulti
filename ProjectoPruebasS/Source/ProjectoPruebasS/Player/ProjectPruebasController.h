// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/PlayerController.h"
#include "ProjectoPruebasS/ProjectPruebaLibrary.h"
#include "ProjectPruebasController.generated.h"

class AProjectoPruebasSCharacter;
/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API AProjectPruebasController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ETeam Team;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Pawns)
	TSubclassOf<AProjectoPruebasSCharacter> ReadTeamPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Pawns)
	TSubclassOf<AProjectoPruebasSCharacter> BlueTeamPawn;
	
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


