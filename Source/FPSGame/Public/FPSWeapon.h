// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSWeapon.generated.h"

class AFPSCharacter;
class USoundBase;

UCLASS()
class FPSGAME_API AFPSWeapon : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AFPSWeapon();

	/** FireWeapon */
	virtual void FireWeapon();

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* FireSound;

	/** Impact FX while bullet hit something */
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem *ImpactFX;

	/** MuzzleFlash FX while weapon is fired */
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem *MuzzleFX;

	UPROPERTY(EditAnywhere)
	float MaxRange = 10000;

	UPROPERTY(EditAnywhere)
	float Damage = 10;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleAnywhere, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

protected:
	/** Get the aim of the weapon, allowing for adjustments to be made by the weapon */
	virtual FVector GetAdjustedAim() const;

	/** Get the originating locaiton for camera damage */
	virtual FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

};
