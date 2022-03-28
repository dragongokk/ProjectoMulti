// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DisplayHud.generated.h"

class AProjectoPruebasSCharacter;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class PROJECTOPRUEBASS_API UDisplayHud : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY( VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Ammo;

	UPROPERTY(Transient)
	AProjectoPruebasSCharacter* MyCharacter;

	float PreviousAmmo;
public:

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UFUNCTION()
	void SetPercentHealth(float health);

	UFUNCTION()
	void SetAmmo(float currentAmmo);
	
};
