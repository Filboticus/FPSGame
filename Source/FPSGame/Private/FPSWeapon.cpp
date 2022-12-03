// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSWeapon.h"
#include "Player/FPSCharacter.h"
#include "Player/FPSPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFPSWeapon::AFPSWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh1P->SetupAttachment(Root);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;

}

void AFPSWeapon::FireWeapon()
{
}

FVector AFPSWeapon::GetAdjustedAim() const
{
	//UE_LOG(LogTemp, Warning, TEXT("GetAdjustedAim()"));
	
	FVector FinalAim = FVector::ZeroVector;

	//AFPSPlayerController* const PlayerController = GetInstigatorController<AFPSPlayerController>();
	APawn* const OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return FinalAim;
	AController* const OwnerController = OwnerPawn->GetController();
	if (OwnerController == nullptr) return FinalAim;

	if (OwnerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController != NULL"));

		FVector CamLoc;
		FRotator CamRot;
		OwnerController->GetPlayerViewPoint(CamLoc, CamRot);

		DrawDebugCamera(GetWorld(), CamLoc, CamRot, 90, 2.0f, FColor::Black, true);
	
		FinalAim = CamRot.Vector();

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController == NULL"));
	}

	return FinalAim;
}

FVector AFPSWeapon::GetCameraDamageStartLocation(const FVector & AimDir) const
{
	FVector OutStartTrace = FVector::ZeroVector;
	
	APawn* const OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return OutStartTrace;
	AController* const OwnerController = OwnerPawn->GetController();
	if (OwnerController == nullptr) return OutStartTrace;

	// use player's Camera
	FRotator UnusedRotation;
	OwnerController->GetPlayerViewPoint(OutStartTrace, UnusedRotation);
	
	// adjust trace so there is nothing blocking the ray between the camera and the pawn
	OutStartTrace = OutStartTrace + AimDir * 25;

	DrawDebugCamera(GetWorld(), OutStartTrace, UnusedRotation, 90, 2.0f, FColor::Red, true);

	return OutStartTrace;
}

FHitResult AFPSWeapon::WeaponTrace(const FVector & StartTrace, const FVector & EndTrace) const
{
	UE_LOG(LogTemp, Warning, TEXT("WeaponTrace()"));
	
	FHitResult Hit;
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECollisionChannel::ECC_GameTraceChannel1);
	if (bSuccess)
	{
		DrawDebugPoint(GetWorld(), StartTrace, 15, FColor::Red, true);
		DrawDebugLine(GetWorld(), StartTrace, Hit.Location, FColor::Red, true);
		DrawDebugPoint(GetWorld(), Hit.Location, 15, FColor::Red, true);
	}
	
	return Hit;
}




