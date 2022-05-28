// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagDomain.h"

#include "ProjectoPruebasSGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/ProjectoPruebasSCharacter.h"

// Sets default values
AFlagDomain::AFlagDomain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	MeshFlag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	MeshFlag->SetupAttachment(RootComponent);
	bReplicates = true;
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this,&AFlagDomain::OnCapsuleBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this,&AFlagDomain::OnCapsuleEndOverlap);
	NumberBlueGuys = 0;
	NumberRedGuys = 0;
	TimePassed = 0;
	bNeutral = true;
}

// Called when the game starts or when spawned
void AFlagDomain::BeginPlay()
{
	Super::BeginPlay();
	if((GetNetMode() == NM_DedicatedServer || GetLocalRole() == ROLE_Authority) && IsValid(GetWorld()))
	{
		AProjectoPruebasSGameMode* MyGamemode = Cast<AProjectoPruebasSGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if(IsValid(MyGamemode))
		{
			MyGamemode->Flags.AddUnique(this);
		}
	}
	VelocityPerPerson = MAxFlagScore / TimeUp;
	if(GetNetMode() == NM_Client)
	{
		TArray<UMaterialInterface*> Materials = MeshFlag->GetMaterials();
		for(int i = 0; i<Materials.Num();++i)
		{
			if(IsValid(Materials[i]))
			{
				InstanceMaterialsDyn.Add(MeshFlag->CreateAndSetMaterialInstanceDynamicFromMaterial(i,Materials[i]));
			}
		}
	}
}

void AFlagDomain::OnRep_Score()
{

	if(Score >= 100 )
	{
		SetMeshColor(FColor::Red);
		return;
	}
	if(Score <= -100)
	{
		SetMeshColor(FColor::Blue);
		return;
	}
	if (bNeutral)
	{
		SetMeshColor(FColor::White);
		return;
	}
	
}

// Called every frame
void AFlagDomain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(GetNetMode() == NM_DedicatedServer || GetNetMode() == NM_ListenServer)
	{
		TimePassed += DeltaTime;
		if(TimePassed >= TimePerTickFlag)
		{
			float Multiplayer = NumberRedGuys - NumberBlueGuys;
			float TempScore = Score;
			TempScore += Multiplayer * VelocityPerPerson * TimePassed;
			TempScore= TempScore >= 100 ? 100:TempScore;
			TempScore = TempScore <= -100 ? -100:TempScore;
			if(abs(TempScore) == MAxFlagScore)
			{
				bNeutral = false;
			}
			if(!(bNeutral) && (Score*TempScore)<= 0)
			{
				bNeutral = true;
			}
			Score = TempScore;
			TimePassed = 0;
		}
	}
	
}


void AFlagDomain::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		const AProjectoPruebasSCharacter* Character = Cast<AProjectoPruebasSCharacter> (OtherActor);
		if(Character)
		{
			switch (Character->Team)
			{
			case ETeam::Red:
				if(NumberRedGuys > 0)
					--NumberRedGuys;	
				break;
			case ETeam::Blue:
				if(NumberBlueGuys > 0)
					--NumberBlueGuys;	
				break;
			default:
				break;
			}
		}
	}
}

void AFlagDomain::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		const AProjectoPruebasSCharacter* Character = Cast<AProjectoPruebasSCharacter> (OtherActor);
		if(Character)
		{
			switch (Character->Team)
			{
			case ETeam::Red:
				++NumberRedGuys;
				break;
			case ETeam::Blue:
				++NumberBlueGuys;
				break;
			default:
				break;
			}
		}
		
	}
	
}


void AFlagDomain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlagDomain, Score);
	DOREPLIFETIME(AFlagDomain, bNeutral);
}

void AFlagDomain::SetMeshColor(FColor Color)
{
	for (UMaterialInstanceDynamic* Mat : InstanceMaterialsDyn)
	{
		Mat->SetVectorParameterValue("Color", Color);
	}
}
