// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "FlagDomain.generated.h"

class UCapsuleComponent;
UCLASS()
class PROJECTOPRUEBASS_API AFlagDomain : public AActor
{
	GENERATED_BODY()



public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = Collider)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* MeshFlag;
	

public:	
	// Sets default values for this actor's properties
	AFlagDomain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
