// Fill out your copyright notice in the Description page of Project Settings.


#include "Data.h"

#include "../Managers/ShooterGameInstance.h"

void UData::Init(UWorld* World)
{
	if(World)
	{
		UShooterGameInstance* MyGameInstance = Cast<UShooterGameInstance>(World->GetGameInstance());
		if(MyGameInstance)
		{
			if(MyGameInstance->BonesDamageMultiplier)
			{
				TArray<FName> RowNames = MyGameInstance->BonesDamageMultiplier->GetRowNames();
				for(FName row :  RowNames)
				{
					FBonesMultiplierData* DataBone =  MyGameInstance->BonesDamageMultiplier->FindRow<FBonesMultiplierData>(row,"");
					BonesMultiplierData.Add(row,*DataBone);
				}
			}
		}
	}
}
