// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSWeapon.h"
#include "FPSWeapon_Instant.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSWeapon_Instant : public AFPSWeapon
{
	GENERATED_BODY()


public:
	
	/** [local] weapon specific fire implementation */	
	virtual void FireWeapon() override;

};
