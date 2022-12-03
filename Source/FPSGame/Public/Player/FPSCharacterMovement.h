// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSCharacterMovement.generated.h"

/**
 *  Movement component meant for use with Pawns.
 */
UCLASS()
class FPSGAME_API UFPSCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	virtual float GetMaxSpeed() const override;
};
