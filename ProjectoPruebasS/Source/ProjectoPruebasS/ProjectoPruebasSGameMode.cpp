// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectoPruebasSGameMode.h"
#include "ProjectoPruebasSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectoPruebasSGameMode::AProjectoPruebasSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
