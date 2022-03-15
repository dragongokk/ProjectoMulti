// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectoPruebasSGameMode.h"

#include "EngineUtils.h"
#include "TeamsManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Player/PlayerStartTeam.h"
#include "Player/ProjectoPruebasSCharacter.h"
#include "UI/ProyectPruebasHud.h"
#include "UObject/ConstructorHelpers.h"

AProjectoPruebasSGameMode::AProjectoPruebasSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = AProyectPruebasHud::StaticClass();
	
}

void AProjectoPruebasSGameMode::PostInitializeComponents()
{
	if(!TeamsManager)
	{
		TeamsManager = NewObject<UTeamsManager>();
	}
	Super::PostInitializeComponents();
}

AActor* AProjectoPruebasSGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	AProjectPruebasController* MyController = Cast<AProjectPruebasController>(Player);
	if(MyController)
	{
		TeamsManager->AddToTeam(MyController);
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			APlayerStartTeam* Spawn = Cast<APlayerStartTeam>(*It);
			if(IsValid(Spawn) && IsValid(Spawn->GetCapsuleComponent()) && Spawn->ControllerOwningThisStart == nullptr && Spawn->Team == MyController->Team)
			{
				Spawn->ControllerOwningThisStart = MyController;
				MyController->SpawnForThisController = Spawn;
				return  Spawn;
			}
		}
	}
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

UClass* AProjectoPruebasSGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	AProjectPruebasController* MyController= Cast<AProjectPruebasController>(InController);
	if(MyController)
	{
		if(MyController->Team == ETeam::Red)
		{
			return MyController->ReadTeamPawn;
		}else if(MyController->Team == ETeam::Blue)
		{
			return MyController->BlueTeamPawn;
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}
