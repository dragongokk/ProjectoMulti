// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BonesMultiplierData.h"
#include "UObject/NoExportTypes.h"
#include "Data.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API UData : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TMap<FName,FBonesMultiplierData> BonesMultiplierData;

public:
	void Init(UWorld* World);
	
};
