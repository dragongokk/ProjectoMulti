// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsManager.h"

#include "Player/ProjectPruebasController.h"


void UTeamsManager::AddToTeam(AProjectPruebasController* CharacterController)
{
	if(IsValid(CharacterController))
	{
		if(!(IsInsideATeam(CharacterController,ETeam::Red) || IsInsideATeam(CharacterController,ETeam::Blue)))
		{
			if(RedTeam.Num() == BlueTeam.Num())
			{
				RedTeam.Add(CharacterController);
				CharacterController->Team = ETeam::Red;
			}
			else
			{
				BlueTeam.Add(CharacterController);
				CharacterController->Team = ETeam::Blue;
			}
		}
	}
}

void UTeamsManager::RemoveToTheTeam(AProjectPruebasController* CharacterController)
{
	if(IsValid(CharacterController))
	{
		if(CharacterController->Team ==  ETeam::Red)
		{
			RedTeam.Remove(CharacterController);
			
		}else if(CharacterController->Team == ETeam::Blue)
		{
			BlueTeam.Remove(CharacterController);
		}
	}
}

bool UTeamsManager::IsInsideATeam(AController* CharacterController,  ETeam Team)
{
	if(Team == ETeam::Red)
	{
		if(RedTeam.Find(CharacterController) != INDEX_NONE )
		{
			return true;
		}
	}else if(Team == ETeam::Blue)
	{
		if(BlueTeam.Find(CharacterController) != INDEX_NONE )
		{
			return true;
		}
	}
	return false;
}


