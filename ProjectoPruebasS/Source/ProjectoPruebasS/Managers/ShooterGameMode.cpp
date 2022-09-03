// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterGameMode.h"

#include "EngineUtils.h"
#include "../Escenario/FlagDomain.h"
#include "ShooterGameState.h"
#include "TeamsManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerStart.h"
#include "../Escenario/PlayerStartTeam.h"
#include "../Jugador/ShooterCharacter.h"
#include "../UI/ShooterHud.h"
#include "UObject/ConstructorHelpers.h"

AShooterGameMode::AShooterGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = AShooterHud::StaticClass();
	
	
}

void AShooterGameMode::PostInitializeComponents()
{
	if(!TeamsManager)
	{
		TeamsManager = NewObject<UTeamsManager>();
	}
	Super::PostInitializeComponents();
	GetWorldTimerManager().SetTimer(AddScoreTimer, this, &AShooterGameMode::AddScore, 1, true);
}

AActor* AShooterGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	AShooterController* MyController = Cast<AShooterController>(Player);
	if(MyController)
	{
		TeamsManager->AddToTeam(MyController);
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			APlayerStartTeam* Spawn = Cast<APlayerStartTeam>(*It);
			if(IsValid(Spawn) && IsValid(Spawn->GetCapsuleComponent()) && Spawn->ControllerOwningThisStart == nullptr && Spawn->Team == MyController->Team && MyController->SpawnForThisController == nullptr)
			{
				Spawn->ControllerOwningThisStart = MyController;
				MyController->SpawnForThisController = Spawn;
				return  Spawn;
			}
		}
	}
//	return Super::FindPlayerStart_Implementation(Player, IncomingName);
	return nullptr;
}

UClass* AShooterGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	AShooterController* MyController= Cast<AShooterController>(InController);
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

void AShooterGameMode::AddScore()
{
	if(!IsValid(MyGameState))
	{
		if(IsValid(GetWorld()))
		{
			MyGameState = Cast<AShooterGameState>(GetWorld()->GetGameState());
		}
	}
	if(IsValid(MyGameState))
	{
		for (int i = 0; i < Flags.Num(); ++i)
		{
			if(IsValid(Flags[i]))
			{
				if(!(Flags[i]->bNeutral))
				{
					if(Flags[i]->Score > 0)
					{
						MyGameState->ScoreRed += 1;
					}else if(Flags[i]->Score < 0)
					{
						MyGameState->ScoreBlue += 1;
					}
				}
			}
		}
	}
}
