// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

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

class AProjectoPruebasSCharacter;
UCLASS()
class PROJECTOPRUEBASS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun2;

	UPROPERTY(Transient,ReplicatedUsing = OnRep_HitInfo)
	FInstantHitInfo OnHitInfo;

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

protected:
	UPROPERTY(Transient)
	AProjectoPruebasSCharacter* OwnerCharacter;

	
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	virtual void Init(AProjectoPruebasSCharacter* Character);

	AProjectoPruebasSCharacter* GetOwner();

	void SimulateShoot(FHitResult hitResult,FTransform RelativeTransForm);

	virtual void OnFire();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	UFUNCTION(server,reliable,WithValidation)
	void ReConfirmHitServer(FHitResult Impact,FTransform RelativeTransform,AProjectoPruebasSCharacter* HitCharacter);
	
	void ProcessHitConfirmed(FHitResult Impact,FTransform RelativeTransform);

	void ComputeBoxValidation(FBox Box,FHitResult Impact,FTransform RelativeTransform);
	
	UFUNCTION()
	virtual void OnRep_HitInfo();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
