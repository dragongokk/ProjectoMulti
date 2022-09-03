// Fill out your copyright notice in the Description page of Project Settings.


#include "DisplayHud.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "../Managers/ShooterGameMode.h"
#include "../Managers/ShooterGameInstance.h"
#include "ProjectoPruebasS/Jugador/ShooterCharacter.h"
#include "ProjectoPruebasS/Jugador/Components/HealthComponent.h"


void UDisplayHud::NativeConstruct()
{
	Super::NativeConstruct();

	SetPercentHealth(100);
	UShooterGameInstance* MyGameInstance = Cast<UShooterGameInstance>(UGameplayStatics::GetGameInstance(this));
	if(IsValid(MyGameInstance))
	{
		MyCharacter= MyGameInstance->GetYourCharacter();
		if(IsValid(MyCharacter))
		{
			MyCharacter->HealthComponent->OnHealthUpdate.AddDynamic(this, &UDisplayHud::SetPercentHealth);
		}
	}
}

void UDisplayHud::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(MyCharacter && MyCharacter->FP_Gun)
	{
		if(PreviousAmmo != MyCharacter->FP_Gun->Ammo)
		{
			SetAmmo(MyCharacter->FP_Gun->Ammo);
			PreviousAmmo = MyCharacter->FP_Gun->Ammo;
		}
		if(!MyGameState)
		{
			AShooterGameState* MyGameSateTemp= Cast<AShooterGameState>(MyCharacter->GetWorld()->GetGameState());
			MyGameState = MyGameSateTemp;
		}
		if(MyGameState)
		{
			SetRedScore(MyGameState->ScoreRed);
			SetBlueScore(MyGameState->ScoreBlue);
		}
	}
}


void UDisplayHud::SetPercentHealth(float health)
{
	HealthBar->SetPercent(health/100.0);
}



void UDisplayHud::SetAmmo(float currentAmmo)
{
	Ammo->SetText(FText::FromString(FString::FromInt(currentAmmo)));
}

void UDisplayHud::SetRedScore(float Score)
{
	RedScore->SetText(FText::FromString(FString::FromInt(Score)));
}

void UDisplayHud::SetBlueScore(float Score)
{
	BlueScore->SetText(FText::FromString(FString::FromInt(Score)));
}
