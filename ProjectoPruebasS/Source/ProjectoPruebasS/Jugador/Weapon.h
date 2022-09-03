// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class AShooterController;
USTRUCT()
struct FInstantHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform RelativeTransform;

	UPROPERTY()
	FHitResult HitResult;

	UPROPERTY()
	bool Shooting = true;
};

class AShooterCharacter;
UCLASS()
class PROJECTOPRUEBASS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun2;
	

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Weapon)
	UParticleSystem* ParticleSystemShoot;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Weapon)
	UParticleSystem* ParticleSystemHit;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Weapon)
	FVector ScaleShootEffect;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Weapon)
	FVector ScaleShootHit;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Weapon)
	bool bDebug;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Weapon)
	float AngleLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float WeaponDamage;

	UPROPERTY(Replicated,Transient)
	int Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxAmmo;

protected:

	FTimerHandle TimerPrueba;
	
	UPROPERTY(Transient)
	AShooterCharacter* OwnerCharacter;

	UPROPERTY(Transient)
	AShooterController* MyController;

	UPROPERTY(Transient,ReplicatedUsing = OnRep_HitInfo)
	FInstantHitInfo OnHitInfo;

	
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	virtual void Init(AShooterCharacter* Character);

	AShooterCharacter* GetOwner();

	void SimulateShoot(FHitResult hitResult,FTransform RelativeTransForm);

	virtual void OnFire();
	
	void Reload();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	UFUNCTION(server,reliable,WithValidation)
	virtual void ReConfirmHitServer(FHitResult Impact,FTransform RelativeTransform,AShooterCharacter* HitCharacter);

	UFUNCTION(Server,Reliable, WithValidation)
	virtual void ReloadServer();
	
	virtual void ProcessHitConfirmed(FHitResult Impact,FTransform RelativeTransform, bool bDamage);

	virtual void ComputeBoxValidation(FBox Box,FHitResult Impact,FHitResult ServerResult,FTransform RelativeTransform); //No usada pero interesante para paliar errores de hit
	
	UFUNCTION()
	virtual void OnRep_HitInfo();
};