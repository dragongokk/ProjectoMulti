// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectPruebasGameInstance.h"

#include "DataTables/Data.h"

void UProjectPruebasGameInstance::Init()
{
	Super::Init();

	if(GetWorld() && (GetWorld()->GetNetMode() == NM_DedicatedServer || GetWorld()->GetNetMode() == NM_ListenServer))
	{
		Data = NewObject<UData>();
		Data->Init(GetWorld());
	}
	
}
