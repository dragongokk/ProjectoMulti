// Fill out your copyright notice in the Description page of Project Settings.


#include "Data.h"

#include "ProjectoPruebasS/ProjectPruebasGameInstance.h"

void UData::Init(UWorld* World)
{
	if(World)
	{
		UProjectPruebasGameInstance* MyGameInstance = Cast<UProjectPruebasGameInstance>(World->GetGameInstance());
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
