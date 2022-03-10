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
		IslocallyControlledDebug = IsLocallyControlled();
		if (IslocallyControlledDebug || GetLocalRole() == ROLE_Authority) 
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

/*
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
*/

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

void AProjectoPruebasSCharacter::BeginDestroy()
{
	DestroyWeapon();
	Super::BeginDestroy();
}

void AProjectoPruebasSCharacter::OnFire()
{
	if (this->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		FP_Gun->OnFire();
	}
}

/*
void AProjectoPruebasSCharacter::SimulateShoot(FHitResult hitResult, FTransform RelativeTransForm)
{
	UGameplayStatics::SpawnEmitterAttached(ParticleSystemShoot, FP_Gun,TEXT("Muzzle"), FVector(ForceInit),
	                                       FRotator::ZeroRotator, ScaleShootEffect);
	if (hitResult.GetActor())

	{
		UGameplayStatics::SpawnEmitterAttached(ParticleSystemHit, hitResult.GetActor()->GetRootComponent(), NAME_None,
		                                       RelativeTransForm.GetLocation(),
		                                       RelativeTransForm.GetRotation().Rotator(), ScaleShootHit/hitResult.GetActor()->GetRootComponent()->GetComponentScale());
	} else if(hitResult.bBlockingHit)
	{
		FTransform TransformEmitter;
		TransformEmitter.SetLocation(hitResult.Location);
		TransformEmitter.SetRotation(hitResult.ImpactNormal.Rotation().Quaternion());
		TransformEmitter.SetScale3D(ScaleShootHit);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ParticleSystemHit,TransformEmitter);
	}
}

void AProjectoPruebasSCharacter::OnRep_HitInfo()
{
	SimulateShoot(OnHitInfo.HitResult, OnHitInfo.RelativeTransform);
}


bool AProjectoPruebasSCharacter::ReConfirmHitServer_Validate(FHitResult Impact, FTransform RelativeTransform, AProjectoPruebasSCharacter* HitCharacter)
{
	return true;
}

void AProjectoPruebasSCharacter::ReConfirmHitServer_Implementation(FHitResult Impact, FTransform RelativeTransform, AProjectoPruebasSCharacter* HitCharacter)
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
	}else
	{
		ProcessHitConfirmed(Impact, FTransform::Identity); //Ha fallado el tiro por lo tanto solo reproducimos el disparo
	}
}

void AProjectoPruebasSCharacter::ProcessHitConfirmed(FHitResult Impact, FTransform RelativeTransform)
{
	
		OnHitInfo.HitResult = Impact;
		OnHitInfo.RelativeTransform = RelativeTransform;
		OnHitInfo.Shooting = !OnHitInfo.Shooting;
}

*/

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
	ReplicatePitch();
}

void AProjectoPruebasSCharacter::ReplicatePitch_Implementation()
{
	FirstPersonCameraComponent->SetWorldRotation(GetViewRotation());
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
