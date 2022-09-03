// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "ShooterCharacter.h"
#include "ShooterController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Managers/ShooterGameInstance.h"
#include "ProjectoPruebasS/DataTables/Data.h"
#include "ProjectoPruebasS/UI/DisplayHud.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	RootComponent = FP_Gun;
	FP_Gun->bOnlyOwnerSee = true; // otherwise won't be visible in multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun2 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun_2"));
	FP_Gun2->bOwnerNoSee = true;

	WeaponDamage = 50.f;


	bReplicates = true;
	//	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bNetUseOwnerRelevancy = true;
}

void AWeapon::Init(AShooterCharacter* Character)
{
	if (IsValid(Character))
	{
		OwnerCharacter = Character;
		SetInstigator(Character);
		SetOwner(Character);
		MyController = Cast<AShooterController>(OwnerCharacter->GetController());
		// For Rpc Calls
	}
}

AShooterCharacter* AWeapon::GetOwner()
{
	return OwnerCharacter;
}

void AWeapon::SimulateShoot(FHitResult hitResult, FTransform RelativeTransForm)
{
	if (OwnerCharacter->IsLocallyControlled())
	{
		UGameplayStatics::SpawnEmitterAttached(ParticleSystemShoot, FP_Gun,TEXT("Muzzle"), FVector(ForceInit),
											   FRotator::ZeroRotator, ScaleShootEffect);
	}
	else
	{
		UGameplayStatics::SpawnEmitterAttached(ParticleSystemShoot, FP_Gun2,TEXT("Muzzle"), FVector(ForceInit),
											   FRotator::ZeroRotator, ScaleShootEffect);
	}
	if (hitResult.GetActor())

	{
		UGameplayStatics::SpawnEmitterAttached(ParticleSystemHit, hitResult.GetActor()->GetRootComponent(), NAME_None,
											   RelativeTransForm.GetLocation(),
											   RelativeTransForm.GetRotation().Rotator(), ScaleShootHit / hitResult.GetActor()->GetRootComponent()->GetComponentScale());
	}
}
/*
	else if (hitResult.bBlockingHit)
	{
		FTransform TransformEmitter;
		TransformEmitter.SetLocation(hitResult.Location);
		TransformEmitter.SetRotation(hitResult.ImpactNormal.Rotation().Quaternion());
		TransformEmitter.SetScale3D(ScaleShootHit);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystemHit, TransformEmitter);
	}
}
*/

void AWeapon::OnFire()
{
	if (OwnerCharacter && OwnerCharacter->IsLocallyControlled() && GetNetMode() == NM_Client && Ammo > 0)
	{
		FHitResult HitResult;
		FTransform CameraTrans = OwnerCharacter->FirstPersonCameraComponent->GetComponentTransform();
		FVector EndPoint = CameraTrans.GetLocation() + (OwnerCharacter->FirstPersonCameraComponent->GetForwardVector() * 10000);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerCharacter);
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
			AShooterCharacter* CharacterHitted = Cast<AShooterCharacter>(HitResult.GetActor());
			SimulateShoot(HitResult, RelativeTransform);
			ReConfirmHitServer(HitResult, RelativeTransform, CharacterHitted);
		}
		else
		{
			ReConfirmHitServer(HitResult, FTransform::Identity, nullptr);
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

void AWeapon::Reload()
{
	ReloadServer();
}


// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	Ammo = MaxAmmo;
}

void AWeapon::ReConfirmHitServer_Implementation(FHitResult Impact, FTransform RelativeTransform,
                                                AShooterCharacter* HitCharacter)
{
	if (Ammo > 0)
	{
		if (OwnerCharacter && HitCharacter && OwnerCharacter->GetMyController() && HitCharacter->GetMyController() &&
			(OwnerCharacter->GetMyController()->Team == HitCharacter->GetMyController()->Team))
		{
			ProcessHitConfirmed(Impact, RelativeTransform, false); //Le damos automaticamente la razón si hemos disparado a un amigo ya que no le vamos a hacer daño
		}
		if (OwnerCharacter && (Impact.GetActor() || Impact.bBlockingHit))
		{
			FVector Origin = OwnerCharacter->FirstPersonCameraComponent->GetComponentLocation(); //Pienso que es mejor tener en cuenta que el disparo realmente se esta realizando desde la camara
			FVector DirVector = (Impact.Location - Origin).GetSafeNormal();

			float viewDot = FVector::DotProduct(OwnerCharacter->FirstPersonCameraComponent->GetComponentRotation().Vector(), DirVector);
			if (bDebug)
			{
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Origin.ToString());
			}
			if (viewDot > AngleLimit) //Vemos si hay demasiado desync (LAG)
			{
				FHitResult HitResult;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(OwnerCharacter);
				FVector ImpactClientLocation = UKismetMathLibrary::TransformLocation(Impact.GetActor()->GetRootComponent()->GetComponentTransform(), RelativeTransform.GetLocation());
				if ((ImpactClientLocation - Impact.Location).Length() <= 20.f)
				{
					FVector EndPoint = ImpactClientLocation + (DirVector * 10000);
					this->GetWorld()->LineTraceSingleByChannel(HitResult, Origin, EndPoint,
					                                           ECC_GameTraceChannel1, Params);
					if (HitResult.GetActor())
					{
						FTransform RelativeTransformServer;
						RelativeTransformServer.SetLocation(UKismetMathLibrary::InverseTransformLocation(
							HitResult.GetActor()->GetRootComponent()->GetComponentTransform(), HitResult.ImpactPoint));
						RelativeTransformServer.SetRotation(UKismetMathLibrary::InverseTransformRotation(
							HitResult.GetActor()->GetRootComponent()->GetComponentTransform(),
							HitResult.ImpactNormal.Rotation()).Quaternion());
						ProcessHitConfirmed(HitResult, RelativeTransformServer, true);
					}
				}
			}
		}
		else
		{
			ProcessHitConfirmed(Impact, FTransform::Identity, false); //Ha fallado el tiro por lo tanto solo reproducimos el disparo
		}
	}
}

bool AWeapon::ReConfirmHitServer_Validate(FHitResult Impact, FTransform RelativeTransform,
                                          AShooterCharacter* HitCharacter)
{
	return true;
}

void AWeapon::ReloadServer_Implementation()
{
	Ammo = MaxAmmo;
}

bool AWeapon::ReloadServer_Validate()
{
	return true;
}

void AWeapon::ProcessHitConfirmed(FHitResult Impact, FTransform RelativeTransform, bool bDamage)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		OnHitInfo.HitResult = Impact;
		OnHitInfo.RelativeTransform = RelativeTransform;
		OnHitInfo.Shooting = !OnHitInfo.Shooting;
		Ammo = Ammo - 1;

		if (bDamage && Impact.GetActor() && Impact.GetActor()->CanBeDamaged())
		{
			UShooterGameInstance* MyGameInstance = Cast<UShooterGameInstance>(GetGameInstance());
			if (MyGameInstance)
			{
				float Damage = WeaponDamage;
				UData* Data = MyGameInstance->Data;
				if (Data)
				{
					FBonesMultiplierData* BoneMultiplierData = Data->BonesMultiplierData.Find(Impact.BoneName);
					if (BoneMultiplierData)
					{
						Damage = BoneMultiplierData->DamageMultiplier * Damage;
					}
				}
				UGameplayStatics::ApplyDamage(Impact.GetActor(), Damage, GetInstigator()->GetController(), this, nullptr);
			}
		}
	}
}

void AWeapon::ComputeBoxValidation(FBox Box, FHitResult Impact, FHitResult ServerResult, FTransform RelativeTransform)
{
	FVector BoxExtent = Box.GetExtent();
	FVector BoxCenter = Box.GetCenter();

	BoxExtent.X = UKismetMathLibrary::Max(20.0f, BoxExtent.X);
	BoxExtent.Y = UKismetMathLibrary::Max(20.0f, BoxExtent.Y);
	BoxExtent.Z = UKismetMathLibrary::Max(20.0f, BoxExtent.Z);

	if (FMath::Abs(Impact.Location.X - BoxCenter.X) < BoxExtent.X &&
		FMath::Abs(Impact.Location.Y - BoxCenter.Y) < BoxExtent.Y &&
		FMath::Abs(Impact.Location.Z - BoxCenter.Z) < BoxExtent.Z)
	{
		ProcessHitConfirmed(Impact, RelativeTransform, true);
	}
	else
	{
		FTransform RelativeTransformServer;
		RelativeTransform.SetLocation(UKismetMathLibrary::InverseTransformLocation(
			ServerResult.GetActor()->GetRootComponent()->GetComponentTransform(), ServerResult.ImpactPoint));
		RelativeTransform.SetRotation(UKismetMathLibrary::InverseTransformRotation(
			ServerResult.GetActor()->GetRootComponent()->GetComponentTransform(),
			ServerResult.ImpactNormal.Rotation()).Quaternion());
		ProcessHitConfirmed(ServerResult, RelativeTransformServer, true);
	}
}

void AWeapon::OnRep_HitInfo()
{
	SimulateShoot(OnHitInfo.HitResult, OnHitInfo.RelativeTransform);
}


void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AWeapon, OnHitInfo, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AWeapon, Ammo, COND_OwnerOnly);
}
