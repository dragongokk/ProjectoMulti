// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectPruebaLibrary.h"
#include "UObject/NoExportTypes.h"
#include "TeamsManager.generated.h"

class AProjectPruebasController;

/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API UTeamsManager : public UObject
{
	GENERATED_BODY()


public:

	UPROPERTY()
	TArray<AController*> BlueTeam;
	
	UPROPERTY()
	TArray<AController*> RedTeam;


public:

	
	/**
	 * @brief Add a character to the corresponding team, if it is already added it will not be added
	 * @param CharacterController Character Controller to add, it had the data of the Team that is in
	 */
	void AddToTeam(AProjectPruebasController* CharacterController);

	
	void RemoveToTheTeam(AProjectPruebasController* CharacterController);

	
	
	
	bool IsInsideATeam(AController* CharacterController, ETeam Team);
};
