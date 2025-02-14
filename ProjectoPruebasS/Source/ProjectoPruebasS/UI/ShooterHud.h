// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHud.generated.h"

class UDisplayHud;
/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API AShooterHud : public AHUD
{
	GENERATED_BODY()


public:
	AShooterHud();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> DisplayHudClass;

	UPROPERTY()
	UDisplayHud* DisplayHud;
	
protected:

	virtual void BeginPlay() override;

	
	
private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
	
};
