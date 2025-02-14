// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Jugador/ShooterController.h"
#include "GameFramework/PlayerStart.h"
#include "ProjectoPruebasS/Librerias/ShooterLibrary.h"
#include "PlayerStartTeam.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API APlayerStartTeam : public APlayerStart
{
	GENERATED_BODY()
public:

	APlayerStartTeam(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Team)
	ETeam Team;

	UPROPERTY(Transient)
	AShooterController* ControllerOwningThisStart = nullptr;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Mesh)
	UStaticMeshComponent* BubleLocate;
	
};
