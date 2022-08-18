// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectoPruebasSCharacter.h"

#include "ProjectPruebasController.h"
#include "../ProjectoPruebasSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/HealthComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "ProjectoPruebasS/ProjectoPruebasSGameMode.h"
#include "ProjectoPruebasS/TeamsManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AProjectoPruebasSCharacter

AProjectoPruebasSCharacter::AProjectoPruebasSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	this->AddOwnedComponent(HealthComponent);
	HealthComponent->SetIsReplicated(true);

	HealthComponent->OnZeroHealth.AddDynamic(this,&AProjectoPruebasSCharacter::OnZeroHealthFunction);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	Mesh1P->bOnlyOwnerSee = true;

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(GetCapsuleComponent());
	Mesh3P->bOwnerNoSee = true;
	

	

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P and FP_Gun
	// are set in the derived blueprint asset named FirstPersonCharacter to avoid direct content references in C++.
}


void AProjectoPruebasSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();



	if(GetLocalRole() == ROLE_Authority)
	{
		FP_Gun  = Cast<AWeapon>(GetWorld()->SpawnActor(GunClass_Initial));
		FP_Gun->Init(this);
		AttachToCharacter();
		
	}

	

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	
}

void AProjectoPruebasSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (InfiValueMove != 0)
	{
		MoveForward(InfiValueMove);
	}
}

void AProjectoPruebasSCharacter::AttachToCharacter()
{
	if(IsValid(FP_Gun))
	{
		FP_Gun->Init(this);
		if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority) 
		{
			FP_Gun->FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false),
									  TEXT("GripPoint"));
		}
		else
		{
			FP_Gun->FP_Gun2->AttachToComponent(Mesh3P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false),TEXT("GripPoint"));
		}
	}
}

void AProjectoPruebasSCharacter::OnRep_WeaponChange()
{
	AttachToCharacter();
}

void AProjectoPruebasSCharacter::DestroyWeapon()
{
	if(FP_Gun->IsValidLowLevel())
	{
		FP_Gun->FP_Gun->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		FP_Gun->FP_Gun2->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		FP_Gun->SetActorHiddenInGame(true);
		if(FP_Gun && GetLocalRole() == ROLE_Authority)
		{
			FP_Gun->Destroy();
		}
	}
}



//////////////////////////////////////////////////////////////////////////// Input

void AProjectoPruebasSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AProjectoPruebasSCharacter::MoveInfiForward(float Val)
{
	if (Val != 0)
	{
		InfiValueMove = Val;
	}
}

void AProjectoPruebasSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ProjectPruebasController = Cast<AProjectPruebasController>(NewController);
	if(ProjectPruebasController)
	{
		Team = ProjectPruebasController->Team;
	}
}

AProjectPruebasController* AProjectoPruebasSCharacter::GetMyController()
{
	return ProjectPruebasController;
}


void AProjectoPruebasSCharacter::OnZeroHealthFunction()
{
	if(GetLocalRole() == ROLE_Authority) //En principio esta función debería ejecutarse si o si en servidor pero por si acaso lo compruebo
	{
		this->SetActorLocation(ProjectPruebasController->SpawnForThisController->GetActorLocation(),true,nullptr,ETeleportType::TeleportPhysics);
		HealthComponent->ResetHealth();
	}
}

void AProjectoPruebasSCharacter::Reload()
{
	if(FP_Gun)
	{
		FP_Gun->Reload();
	}
}

FRotator AProjectoPruebasSCharacter::GetAimView()
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void AProjectoPruebasSCharacter::BeginDestroy()
{
	DestroyWeapon();
	if(GetLocalRole() == ROLE_Authority)
	{
		AProjectoPruebasSGameMode* MyGamemode =Cast<AProjectoPruebasSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if(MyGamemode)
		{
			MyGamemode->TeamsManager->RemoveToTheTeam(Cast<AProjectPruebasController>(this->GetController()));
		}
	}
	Super::BeginDestroy();
}



void AProjectoPruebasSCharacter::OnFire()
{
	if (this->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		FP_Gun->OnFire();
	}
}

void AProjectoPruebasSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AProjectoPruebasSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AProjectoPruebasSCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	
	
}

void AProjectoPruebasSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	ReplicatePitch(FirstPersonCameraComponent->GetComponentRotation());
}

void AProjectoPruebasSCharacter::ReplicatePitch_Implementation(FRotator Rotation)
{
	FirstPersonCameraComponent->SetWorldRotation(Rotation);
}

float AProjectoPruebasSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                             AController* EventInstigator, AActor* DamageCauser)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		HealthComponent->UpdateHealth(-DamageAmount);
	}

	return DamageAmount;
}

void AProjectoPruebasSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectoPruebasSCharacter, FP_Gun);
}
