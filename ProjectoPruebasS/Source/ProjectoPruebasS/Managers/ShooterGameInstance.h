// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShooterGameState.h"


#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

class AShooterCharacter;
class UData;
class UDataTable;
/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = DataTables)
	UDataTable* BonesDamageMultiplier;

	UPROPERTY()
	UData* Data;

	

private:
	
	UPROPERTY() // El servidor tendra el character que es el controlador 0 por lo tanto no vale para localzar el character en el servidor ( no hay)
	AShooterCharacter* YourCharacter;

public:
	virtual void Init() override;

	virtual AShooterCharacter* GetYourCharacter();
	
	
};
