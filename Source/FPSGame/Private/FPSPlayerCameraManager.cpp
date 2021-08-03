// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "FPSPlayerCameraManager.h"

AFPSPlayerCameraManager::AFPSPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 60.0f;
}

void AFPSPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	AFPSCharacter* MyPawn = PCOwner ? Cast<AFPSCharacter>(PCOwner->GetPawn()) : NULL;
	if (MyPawn)
	{
		const float TargetFOV = MyPawn->IsTargeting() ? TargetingFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
	}

	Super::UpdateCamera(DeltaTime);

	if (MyPawn)
	{
		MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation());
	}

}