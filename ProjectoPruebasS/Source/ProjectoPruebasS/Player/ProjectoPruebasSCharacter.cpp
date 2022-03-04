// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectoPruebasSCharacter.h"
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

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AProjectoPruebasSCharacter

AProjectoPruebasSCharacter::AProjectoPruebasSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	bDebug = true;
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	ScaleShootEffect = FVector(1, 1, 1);
	ScaleShootHit = FVector(1, 1, 1);
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	this->AddOwnedComponent(HealthComponent);
	HealthComponent->SetIsReplicated(true);

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


	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false); // otherwise won't be visible in multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);


	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P and FP_Gun
	// are set in the derived blueprint asset named FirstPersonCharacter to avoid direct content references in C++.
}


void AProjectoPruebasSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	if (IsLocallyControlled())
	{
		FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		                          TEXT("GripPoint"));
	}
	else
	{
		FP_Gun->AttachToComponent(Mesh3P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		                          TEXT("GripPoint"));
	}
}

void AProjectoPruebasSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (InfiValueMove != 0)
	{
		MoveForward(InfiValueMove);
	}
}


//////////////////////////////////////////////////////////////////////////// Input

void AProjectoPruebasSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AProjectoPruebasSCharacter::OnFire);


	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AProjectoPruebasSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveInfiForward", this, &AProjectoPruebasSCharacter::MoveInfiForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProjectoPruebasSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AProjectoPruebasSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AProjectoPruebasSCharacter::LookUpAtRate);
}

void AProjectoPruebasSCharacter::ComputeBoxValidation(FBox Box,FHitResult Impact,FTransform RelativeTransform)
{
	FVector BoxExtent = Box.GetExtent();
	FVector BoxCenter = Box.GetCenter();

	BoxExtent.X = UKismetMathLibrary::Max(20.0f,BoxExtent.X);
	BoxExtent.Y = UKismetMathLibrary::Max(20.0f,BoxExtent.Y);
	BoxExtent.Z = UKismetMathLibrary::Max(20.0f,BoxExtent.Z);

	if(	FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
		FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y &&
		FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z)
	{
		ProcessHitConfirmed(Impact,RelativeTransform);
	}
}

void AProjectoPruebasSCharacter::MoveInfiForward(float Val)
{
	if (Val != 0)
	{
		InfiValueMove = Val;
	}
}

FRotator AProjectoPruebasSCharacter::GetAimView()
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void AProjectoPruebasSCharacter::OnFire()
{
	if (this->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		FHitResult HitResult;
		FTransform CameraTrans = FirstPersonCameraComponent->GetComponentTransform();
		FVector EndPoint = CameraTrans.GetLocation() + (FirstPersonCameraComponent->GetForwardVector() * 10000);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		this->GetWorld()->LineTraceSingleByChannel(HitResult, CameraTrans.GetLocation(), EndPoint,
		                                           ECC_GameTraceChannel1, Params);
		if (IsValid(HitResult.GetActor()) && HitResult.GetActor()->GetRemoteRole() == ROLE_Authority)
		{
			//We hit a character so where are going to try a precise shoot

			FTransform RelativeTransform;
			RelativeTransform.SetLocation(UKismetMathLibrary::InverseTransformLocation(
				HitResult.GetActor()->GetRootComponent()->GetComponentTransform(), HitResult.ImpactPoint));
			RelativeTransform.SetRotation(UKismetMathLibrary::InverseTransformRotation(
				HitResult.GetActor()->GetRootComponent()->GetComponentTransform(),
				HitResult.ImpactNormal.Rotation()).Quaternion());
			SimulateShoot(HitResult, RelativeTransform);
			ConfirmHitServer(HitResult, RelativeTransform,Cast<AProjectoPruebasSCharacter>(HitResult.GetActor()));
		}
		else
		{
			if (HitResult.bBlockingHit)
			{
				ConfirmHitServer(HitResult, FTransform::Identity,nullptr);
			}
			else
			{
				//MissHitServer();
			}
		}

		if (bDebug)
		{
			DrawDebugLine(GetWorld(), CameraTrans.GetLocation(), HitResult.ImpactPoint, FColor::Red, false, 10);
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 20, 32, FColor::Red, false, 10);
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, HitResult.BoneName.ToString());
		}
	}
}

void AProjectoPruebasSCharacter::SimulateShoot(FHitResult hitResult, FTransform RelativeTransForm)
{
	UGameplayStatics::SpawnEmitterAttached(ParticleSystemShoot, FP_Gun,TEXT("Muzzle"), FVector(ForceInit),
	                                       FRotator::ZeroRotator, ScaleShootEffect);
	if (hitResult.GetActor())

	{
		UGameplayStatics::SpawnEmitterAttached(ParticleSystemHit, hitResult.GetActor()->GetRootComponent(), NAME_None,
		                                       RelativeTransForm.GetLocation(),
		                                       RelativeTransForm.GetRotation().Rotator(), ScaleShootHit/hitResult.GetActor()->GetRootComponent()->GetComponentScale());
	}
}

void AProjectoPruebasSCharacter::OnRep_HitInfo()
{
	SimulateShoot(OnHitInfo.HitResult, OnHitInfo.RelativeTransform);
}


bool AProjectoPruebasSCharacter::ConfirmHitServer_Validate(FHitResult Impact, FTransform RelativeTransform, AProjectoPruebasSCharacter* HitCharacter)
{
	return true;
}

void AProjectoPruebasSCharacter::ConfirmHitServer_Implementation(FHitResult Impact, FTransform RelativeTransform, AProjectoPruebasSCharacter* HitCharacter)
{
	if(Impact.GetActor() || Impact.bBlockingHit)
	{
		FVector Origin = FP_Gun->GetSocketLocation("Muzzle"); //Posible método para conseguir la localizancion del comienzo del disparo
		FVector DirVector = (Impact.Location - Origin).GetSafeNormal();

		float viewDot = FVector::DotProduct(GetViewRotation().Vector(),DirVector);
		if(viewDot > AngleLimit)
		{
			if(!(Impact.GetActor()) && Impact.bBlockingHit)
			{
				ProcessHitConfirmed(Impact,RelativeTransform);
			}else if(Impact.GetActor())
			{
				if(Impact.GetActor()->IsRootComponentStatic() || Impact.GetActor()->IsRootComponentStationary())
				{
					ProcessHitConfirmed(Impact,RelativeTransform); //Le damos la razón automaticamente si el objeto es estático
				}else
				{
					if(!(HitCharacter))
					{
						const FBox Box = Impact.GetActor()->GetComponentsBoundingBox();

						ComputeBoxValidation(Box,Impact,RelativeTransform);
							
					}else
					{
						const FBox Box = HitCharacter->Mesh3P->GetBodyInstance(Impact.BoneName)->GetBodyBounds();

						ComputeBoxValidation(Box,Impact,RelativeTransform);
					}
				}
			}
		}
	}
}

void AProjectoPruebasSCharacter::ProcessHitConfirmed(FHitResult Impact, FTransform RelativeTransform)
{
	if (IsValid(Impact.GetActor()))
	{
		OnHitInfo.HitResult = Impact;
		OnHitInfo.RelativeTransform = RelativeTransform;
		OnHitInfo.Shooting = !OnHitInfo.Shooting;
	}
}



//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AProjectoPruebasSCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

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
}

float AProjectoPruebasSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                             AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	HealthComponent->UpdateHealth(-DamageAmount);

	return DamageAmount;
}

void AProjectoPruebasSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AProjectoPruebasSCharacter, OnHitInfo, COND_SkipOwner);
}
