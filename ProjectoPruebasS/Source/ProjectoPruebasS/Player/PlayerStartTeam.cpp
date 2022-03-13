// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStartTeam.h"

APlayerStartTeam::APlayerStartTeam(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	BubleLocate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Buble"));
	BubleLocate->SetupAttachment(RootComponent);
}
