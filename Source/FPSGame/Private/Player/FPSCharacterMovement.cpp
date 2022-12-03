// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FPSCharacter.h"
#include "Player/FPSCharacterMovement.h"

//----------------------------------------------------------------------//
// UPawnMovementComponent
//----------------------------------------------------------------------//

float UFPSCharacterMovement::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const AFPSCharacter* FPSCharacterOwner = Cast<AFPSCharacter>(PawnOwner);
	if (FPSCharacterOwner)
	{
		if (FPSCharacterOwner->IsTargeting())
		{
			//ToDo: Delcare GetTargetingSpeedModifier in FPSCharacter
			MaxSpeed *= FPSCharacterOwner->GetTargetingSpeedModifier();
		}
		if (FPSCharacterOwner->IsRunning())
		{
			//ToDo: Delcare GetRunningSpeedModifier in FPSCharacter
			MaxSpeed *= FPSCharacterOwner->GetRunningSpeedModifier();
		}
	}

	return MaxSpeed;
}