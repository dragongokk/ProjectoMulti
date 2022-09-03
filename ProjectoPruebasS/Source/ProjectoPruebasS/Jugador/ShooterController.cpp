// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterController.h"

#include "ShooterCharacter.h"
#include "ProjectoPruebasS/UI/ShooterHud.h"

void AShooterController::BeginPlay()
{
	Super::BeginPlay();
	MainCharacter = Cast<AShooterCharacter>(GetPawn());
}

void AShooterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Jump", IE_Pressed, this, &AShooterController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &AShooterController::StopJumping);

	// Bind fire event
	InputComponent->BindAction("Fire", IE_Pressed, this, &AShooterController::OnFire);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AShooterController::Reload);


	// Bind movement events
	InputComponent->BindAxis("MoveForward", this, &AShooterController::MoveForward);
	InputComponent->BindAxis("MoveInfiForward", this, &AShooterController::MoveInfiForward);
	InputComponent->BindAxis("MoveRight", this, &AShooterController::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &AShooterController::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AShooterController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AShooterController::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AShooterController::LookUpAtRate);
	
}

void AShooterController::LookUpAtRate(float X)
{
	if(MainCharacter)
	{
		MainCharacter->LookUpAtRate(X);
	}
}

void AShooterController::TurnAtRate(float X)
{
	if(MainCharacter)
	{
		MainCharacter->TurnAtRate(X);
	}
}

void AShooterController::OnFire()
{
	if(MainCharacter)
	{
		MainCharacter->OnFire();
	}
}

void AShooterController::MoveForward(float X)
{
	if(MainCharacter)
	{
		MainCharacter->MoveForward(X);
	}
}

void AShooterController::MoveInfiForward(float X)
{
	if(MainCharacter)
	{
		MainCharacter->MoveInfiForward(X);
	}
}

void AShooterController::MoveRight(float X)
{
	if(MainCharacter)
	{
		MainCharacter->MoveRight(X);
	}
}

void AShooterController::Jump()
{
	if(MainCharacter)
	{
		MainCharacter->Jump();
	}
}

void AShooterController::StopJumping()
{
	if(MainCharacter)
	{
		MainCharacter->StopJumping();
	}
}


void AShooterController::AddControllerYawInput(float X)
{
	if(MainCharacter)
	{
		MainCharacter->AddControllerYawInput(X);
	}
}


void AShooterController::AddControllerPitchInput(float X)
{
	if(MainCharacter)
	{
		MainCharacter->AddControllerPitchInput(X);
	}
}

void AShooterController::Reload()
{
	if(MainCharacter)
	{
		MainCharacter->Reload();
	}
}
