// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectPruebasController.h"

#include "ProjectoPruebasSCharacter.h"
#include "ProjectoPruebasS/UI/ProyectPruebasHud.h"

void AProjectPruebasController::BeginPlay()
{
	Super::BeginPlay();
	MainCharacter = Cast<AProjectoPruebasSCharacter>(GetPawn());
}

void AProjectPruebasController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Jump", IE_Pressed, this, &AProjectPruebasController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &AProjectPruebasController::StopJumping);

	// Bind fire event
	InputComponent->BindAction("Fire", IE_Pressed, this, &AProjectPruebasController::OnFire);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AProjectPruebasController::Reload);


	// Bind movement events
	InputComponent->BindAxis("MoveForward", this, &AProjectPruebasController::MoveForward);
	InputComponent->BindAxis("MoveInfiForward", this, &AProjectPruebasController::MoveInfiForward);
	InputComponent->BindAxis("MoveRight", this, &AProjectPruebasController::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &AProjectPruebasController::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AProjectPruebasController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AProjectPruebasController::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AProjectPruebasController::LookUpAtRate);
	
}

void AProjectPruebasController::LookUpAtRate(float X)
{
	if(MainCharacter)
	{
		MainCharacter->LookUpAtRate(X);
	}
}

void AProjectPruebasController::TurnAtRate(float X)
{
	if(MainCharacter)
	{
		MainCharacter->TurnAtRate(X);
	}
}

void AProjectPruebasController::OnFire()
{
	if(MainCharacter)
	{
		MainCharacter->OnFire();
	}
}

void AProjectPruebasController::MoveForward(float X)
{
	if(MainCharacter)
	{
		MainCharacter->MoveForward(X);
	}
}

void AProjectPruebasController::MoveInfiForward(float X)
{
	if(MainCharacter)
	{
		MainCharacter->MoveInfiForward(X);
	}
}

void AProjectPruebasController::MoveRight(float X)
{
	if(MainCharacter)
	{
		MainCharacter->MoveRight(X);
	}
}

void AProjectPruebasController::Jump()
{
	if(MainCharacter)
	{
		MainCharacter->Jump();
	}
}

void AProjectPruebasController::StopJumping()
{
	if(MainCharacter)
	{
		MainCharacter->StopJumping();
	}
}


void AProjectPruebasController::AddControllerYawInput(float X)
{
	if(MainCharacter)
	{
		MainCharacter->AddControllerYawInput(X);
	}
}


void AProjectPruebasController::AddControllerPitchInput(float X)
{
	if(MainCharacter)
	{
		MainCharacter->AddControllerPitchInput(X);
	}
}

void AProjectPruebasController::Reload()
{
	if(MainCharacter)
	{
		MainCharacter->Reload();
	}
}
