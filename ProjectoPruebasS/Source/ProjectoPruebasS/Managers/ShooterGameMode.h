// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"



#include "GameFramework/GameModeBase.h"
#include "ShooterGameMode.generated.h"

class AFlagDomain;
class AShooterGameState;
class UTeamsManager;
UCLASS(minimalapi)
class AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(NoClear,Transient)
	UTeamsManager* TeamsManager;

	UPROPERTY()
	TArray<AFlagDomain*> Flags;
	
	UPROPERTY()
	AShooterGameState* MyGameState;
	
	
private:

	FTimerHandle AddScoreTimer;

	
public:
	AShooterGameMode();
	virtual void PostInitializeComponents() override;
	
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT("")) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	void AddScore();

};



