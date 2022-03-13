// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "ProjectoPruebasSGameMode.generated.h"

class UTeamsManager;
UCLASS(minimalapi)
class AProjectoPruebasSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(NoClear,Transient)
	UTeamsManager* TeamsManager;
	
public:
	AProjectoPruebasSGameMode();
	virtual void PostInitializeComponents() override;
	
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT("")) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
};



