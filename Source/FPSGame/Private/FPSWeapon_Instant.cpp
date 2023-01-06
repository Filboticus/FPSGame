// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSWeapon_Instant.h"
#include "FPSWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FPSCharacter.h"

void AFPSWeapon_Instant::FireWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("FPSWeapon_Instant::FireWeapon()"));
	
	const FVector AimDir = GetAdjustedAim();
	UE_LOG(LogTemp, Warning, TEXT("AimDir: %s"), *AimDir.ToString());
	
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	UE_LOG(LogTemp, Warning, TEXT("StartTrace: %s"), *StartTrace.ToString());

	FVector EndTrace = StartTrace + AimDir * MaxRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	
	APawn* const OwnerPawn = Cast<APawn>(GetOwner());
	AController* const OwnerController = OwnerPawn->GetController();

	//Damage 
	AActor* HitActor = Impact.GetActor();
	if (HitActor != nullptr)
	{
		FPointDamageEvent DamageEvent(Damage, Impact, AimDir, nullptr);
		HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
	}

	
	// Spawn Impact FX at the location of the Hit
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, Impact.Location, StartTrace.Rotation());

	// try and play the sound if specified
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	
	

}

