// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "ProjectoPruebasS/Librerias/ShooterLibrary.h"
#include "ShooterController.generated.h"

class AShooterHud;
class AShooterCharacter;
/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API AShooterController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ETeam Team;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Pawns)
	TSubclassOf<AShooterCharacter> ReadTeamPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Pawns)
	TSubclassOf<AShooterCharacter> BlueTeamPawn;

	UPROPERTY()
	APlayerStart* SpawnForThisController;

	
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

	void Reload();
	
	virtual void SetupInputComponent() override;

	UPROPERTY()
	AShooterCharacter* MainCharacter;
	
};


