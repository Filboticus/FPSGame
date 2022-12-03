// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FPSPlayerController.h"
#include "Player/FPSPlayerCameraManager.h"

AFPSPlayerController::AFPSPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AFPSPlayerCameraManager::StaticClass();
}

/* void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

		// UI input
		//InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AFPSPlayerController::OnToggleInGameMenu);
}

void AFPSPlayerController::OnToggleInGameMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Escape Pressed to Access InGameMenu"));

	
} */