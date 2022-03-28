// Fill out your copyright notice in the Description page of Project Settings.


#include "DisplayHud.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectoPruebasS/ProjectPruebasGameInstance.h"
#include "ProjectoPruebasS/Player/ProjectoPruebasSCharacter.h"
#include "ProjectoPruebasS/Player/Components/HealthComponent.h"


void UDisplayHud::NativeConstruct()
{
	Super::NativeConstruct();

	SetPercentHealth(100);
	UProjectPruebasGameInstance* MyGameInstance = Cast<UProjectPruebasGameInstance>(UGameplayStatics::GetGameInstance(this));
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
