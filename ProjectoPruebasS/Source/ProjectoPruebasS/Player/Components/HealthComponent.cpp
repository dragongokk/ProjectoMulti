// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//Ver despues si es necesario la replicacion
	// ...
	MaxHealth = 0;
}



void UHealthComponent::UpdateHealth(float Amount)
{
	if(GetOwner()->GetLocalRole() == ENetRole::ROLE_Authority)
	{
		float tempHealth= CurrentHealth+Amount;
		if(tempHealth > MaxHealth)
		{
			CurrentHealth = MaxHealth;
			return;
		}if(tempHealth <= 0)
		{
			//Ded
			CurrentHealth = 0;
			return;
		}
		CurrentHealth +=Amount;
	}
}

void UHealthComponent::OnRep_CurrentHealth()
{
	//Only the owner of the pawn will notify to their components that the health has been updated
	if(GetOwner() && GetOwner()->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		OnHealthUpdate.Broadcast(CurrentHealth);
	}
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(CurrentHealth));  
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
    CurrentHealth = MaxHealth;
	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent,CurrentHealth);
	DOREPLIFETIME(UHealthComponent,MaxHealth);
	
}

