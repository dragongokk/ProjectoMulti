// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GameFramework/Character.h"
#include "ProjectoPruebasS/Librerias/ShooterLibrary.h"
#include "ShooterCharacter.generated.h"

class AShooterController;
class AShooterCharacter;
class UHealthComponent;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;



UCLASS(config=Game)
class AShooterCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh3P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = Weapon)
	TSubclassOf<AWeapon> GunClass_Initial;
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_WeaponChange)
	AWeapon* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	UHealthComponent* HealthComponent;

	/*
	UPROPERTY(Transient,ReplicatedUsing = OnRep_HitInfo)
	FInstantHitInfo OnHitInfo;
	*/
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	UPROPERTY(Replicated)
	ETeam Team;

protected:
	UPROPERTY()
	AShooterController* ProjectPruebasController;

private:
	float InfiValueMove = 0;

	
public:
	AShooterCharacter();
	

	UFUNCTION(BlueprintCallable)
	FRotator GetAimView();

	virtual void BeginDestroy() override;
	
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	//void SimulateShoot(FHitResult hitResult,FTransform RelativeTransForm);

	void MoveInfiForward(float Val);

	virtual void PossessedBy(AController* NewController) override;

	AShooterController* GetMyController();

	bool IsDead();

	UFUNCTION()
	void OnZeroHealthFunction();
	
	void Reload();

protected:

	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

	void AttachToCharacter();

	UFUNCTION()
	void OnRep_WeaponChange();

	void DestroyWeapon();

	

	
	/*
	UFUNCTION()
	virtual void OnRep_HitInfo();

	
	UFUNCTION(Server,Reliable,WithValidation)
	void ReConfirmHitServer(FHitResult Impact,FTransform RelativeTransform,AProjectoPruebasSCharacter* HitCharacter);
	*/

	//void ProcessHitConfirmed(FHitResult Impact,FTransform RelativeTransform);
	
	/** Fires a projectile. */
	

	UFUNCTION(Server,Unreliable)
	void ReplicatePitch(FRotator Rotation);
	
	

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

//	void ComputeBoxValidation(FBox Box,FHitResult Impact,FTransform RelativeTransform);
	

};




