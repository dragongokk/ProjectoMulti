// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

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
	
	UPROPERTY(Transient,Replicated,EditAnywhere,BlueprintReadWrite,Category = Config)
	float MaxHealth;

	UPROPERTY(Transient,ReplicatedUsing=OnRep_CurrentHealth)
	float CurrentHealth;

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	void UpdateHealth(float Amount);

	UFUNCTION()
	void OnRep_CurrentHealth();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
