// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HealthComponent.generated.h"

class AProjectoPruebasSCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthUpdate,float, CurrentHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnZeroHealth);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOPRUEBASS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintAssignable, Category="OnHealthUpdate")
	FOnHealthUpdate OnHealthUpdate;

	UPROPERTY(BlueprintAssignable, Category="ZeroHealth")
	FOnZeroHealth OnZeroHealth;
	
	UPROPERTY(Replicated,EditAnywhere,BlueprintReadWrite,Category = Config)
	float MaxHealth;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Debug)
	bool bDebug;

protected:
	UPROPERTY(Transient,ReplicatedUsing=OnRep_CurrentHealth)
	float CurrentHealth;

	UPROPERTY(Transient)
	AProjectoPruebasSCharacter* OwnerCharacter;

public:	
	// Sets default values for this component's properties
	UHealthComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetHealth();

	void ResetHealth();

	void UpdateHealth(float Amount);
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	

	UFUNCTION()
	void OnRep_CurrentHealth();



		
};
