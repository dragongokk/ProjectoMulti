// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameInstance.h"

#include "../DataTables/Data.h"
#include "Kismet/GameplayStatics.h"
#include "../Jugador/ShooterCharacter.h"

void UShooterGameInstance::Init()
{
	Super::Init();

	if(GetWorld() && (GetWorld()->GetNetMode() == NM_DedicatedServer || GetWorld()->GetNetMode() == NM_ListenServer))
	{
		Data = NewObject<UData>();
		Data->Init(GetWorld());
	}
	
}

AShooterCharacter* UShooterGameInstance::GetYourCharacter()
{
	if(!IsValid(YourCharacter))
	{
		YourCharacter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	}
	return YourCharacter;
}


