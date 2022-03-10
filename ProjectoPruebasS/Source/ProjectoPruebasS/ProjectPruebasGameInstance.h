// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameInstance.h"
#include "ProjectPruebasGameInstance.generated.h"

class UData;
class UDataTable;
/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API UProjectPruebasGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = DataTables)
	UDataTable* BonesDamageMultiplier;

	UPROPERTY()
	UData* Data;

public:
	virtual void Init() override;
};
