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

	UPROPERTY(ReplicatedUsing = OnRep_Score)
	float Score;

	//Tiempo que tarda de llegar de 0 a 100 con una persona, no tiene en cuenta si esta en el negativo o las demas personas, en segundos
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Config) 
	float TimeUp;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Config) 
	float TimePerTickFlag;
	
	float NumberBlueGuys;
	
	float NumberRedGuys;
	
	UPROPERTY(Replicated)
	bool bNeutral;
	

protected:
	UPROPERTY(Transient)
	TArray<UMaterialInstanceDynamic*> InstanceMaterialsDyn;


private:
	const int MAxFlagScore = 100;

	float VelocityPerPerson;

	float TimePassed;
	
	float previousScore;


public:	
	// Sets default values for this actor's properties
	AFlagDomain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnRep_Score();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult);
	
	void SetMeshColor(FColor Color);

	UFUNCTION()
	virtual void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	

};


