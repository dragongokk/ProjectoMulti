// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagDomain.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AFlagDomain::AFlagDomain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	MeshFlag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	MeshFlag->SetupAttachment(RootComponent);
	bReplicates = true;
	
}

// Called when the game starts or when spawned
void AFlagDomain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlagDomain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

