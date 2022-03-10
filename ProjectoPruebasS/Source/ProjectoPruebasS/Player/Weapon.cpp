// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "HistoryManager.h"
#include "ProjectoPruebasSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectoPruebasS/ProjectPruebasGameInstance.h"
#include "ProjectoPruebasS/DataTables/Data.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

void AWeapon::Init(AProjectoPruebasSCharacter* Character)
{
	if(IsValid(Character))
	{
		OwnerCharacter = Character;
		SetInstigator(Character);
		SetOwner(Character);
		// For Rpc Calls
	}
	
}

AProjectoPruebasSCharacter* AWeapon::GetOwner()
{
	return OwnerCharacter;
}

void AWeapon::SimulateShoot(FHitResult hitResult, FTransform RelativeTransForm)
{
	if(OwnerCharacter->IsLocallyControlled())
	{
		UGameplayStatics::SpawnEmitterAttached(ParticleSystemShoot, FP_Gun,TEXT("Muzzle"), FVector(ForceInit),
											   FRotator::ZeroRotator, ScaleShootEffect);
	}else
	{
		UGameplayStatics::SpawnEmitterAttached(ParticleSystemShoot, FP_Gun2,TEXT("Muzzle"), FVector(ForceInit),
											   FRotator::ZeroRotator, ScaleShootEffect);
	}
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

void AWeapon::OnFire()
{
	if (OwnerCharacter && OwnerCharacter->IsLocallyControlled() && GetNetMode() == NM_Client )
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
			SimulateShoot(HitResult, RelativeTransform);
			ReConfirmHitServer(HitResult, RelativeTransform,Cast<AProjectoPruebasSCharacter>(HitResult.GetActor()));
		}
		else
		{
			if (HitResult.bBlockingHit)
			{
				ReConfirmHitServer(HitResult, FTransform::Identity,nullptr);
			}
			else
			{
				ReConfirmHitServer(HitResult, FTransform::Identity,nullptr);
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


// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::ReConfirmHitServer_Implementation(FHitResult Impact, FTransform RelativeTransform,
	AProjectoPruebasSCharacter* HitCharacter)
{
	
	if(OwnerCharacter && (Impact.GetActor() || Impact.bBlockingHit))
	{
		FVector Origin = FP_Gun->GetSocketLocation("Muzzle"); //Posible método para conseguir la localizancion del comienzo del disparo
		FVector DirVector = (Impact.Location - Origin).GetSafeNormal();

		float viewDot = FVector::DotProduct(OwnerCharacter->GetViewRotation().Vector(),DirVector);
		if(bDebug)
		{
			if(GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Origin.ToString());
		}
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

bool AWeapon::ReConfirmHitServer_Validate(FHitResult Impact, FTransform RelativeTransform,
	AProjectoPruebasSCharacter* HitCharacter)
{
	return true;
}

void AWeapon::ProcessHitConfirmed(FHitResult Impact, FTransform RelativeTransform)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		OnHitInfo.HitResult = Impact;
		OnHitInfo.RelativeTransform = RelativeTransform;
		OnHitInfo.Shooting = !OnHitInfo.Shooting;

		if(Impact.GetActor() && Impact.GetActor()->CanBeDamaged())
		{
			UProjectPruebasGameInstance* MyGameInstance = Cast<UProjectPruebasGameInstance>(GetGameInstance());
			if(MyGameInstance)
			{
				UData* Data = MyGameInstance->Data;
				if(Data)
				{
					FBonesMultiplierData* BoneMultiplierData= Data->BonesMultiplierData.Find(Impact.BoneName);
					if(BoneMultiplierData)
					{
						float Damage = BoneMultiplierData->DamageMultiplier * WeaponDamage;
						UGameplayStatics::ApplyDamage(Impact.GetActor(),Damage,GetInstigator()->GetController(),this,nullptr);
					}
				}
			}
		}
	}
}

void AWeapon::ComputeBoxValidation(FBox Box, FHitResult Impact, FTransform RelativeTransform)
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

void AWeapon::OnRep_HitInfo()
{
	SimulateShoot(OnHitInfo.HitResult, OnHitInfo.RelativeTransform);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AWeapon, OnHitInfo, COND_SkipOwner);
}

