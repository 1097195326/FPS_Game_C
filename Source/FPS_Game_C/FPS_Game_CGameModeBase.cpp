// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Game_C.h"
#include "FPS_Game_CGameModeBase.h"

void AFPS_Game_CGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, TEXT("Hello World, This is FPSGameMode"));
	}

}


