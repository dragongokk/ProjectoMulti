// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHud.h"

#include "CanvasItem.h"
#include "DisplayHud.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Canvas.h"

AShooterHud::AShooterHud()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}

void AShooterHud::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	if(GetNetMode() == NM_Client)
	{
		const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
		const FVector2D CrosshairDrawPosition( (Center.X) -8.0f,
											   (Center.Y)-8.0f);
	

		// draw the crosshair
		FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem( TileItem );
	}
}

void AShooterHud::BeginPlay()
{
	Super::BeginPlay();

	if(DisplayHudClass && GetNetMode() == NM_Client)
	{
		if(!DisplayHud)
		{
			DisplayHud = CreateWidget<UDisplayHud>(GetWorld(),DisplayHudClass);
			DisplayHud->AddToViewport();
		}
	}
}
