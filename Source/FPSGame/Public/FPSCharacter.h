// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSWeapon.h"
#include "FPSWeapon_Instant.h"
#include "FPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class AFPSProjectile;
class USoundBase;
class UAnimSequence;
class AFPSWeapon;


UCLASS()
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** get max health */
	int32 GetMaxHealth() const;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private: 
	/** Pawn mesh: 1st person view  */
	UPROPERTY(VisibleDefaultsOnly, Category="Mesh")
	USkeletalMeshComponent* Mesh1PComponent;



	/** Override TakeDage from Actor.h */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	
public:
	AFPSCharacter();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<AFPSProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	UAnimSequence* FireAnimation;

	/** Add camera pitch to first person mesh. 
	*	
	*	@param CameraLocation Location of the Camera.
	*	@Param CameraRotation Rotation of the Camera.
	*
	*/
	void OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation);

	/** Get targeting state */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	bool IsTargeting() const;

	// Current Health of the Pawn
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	/** Get either first or third person mesh. 
	*
	* @param	WantFirstPerson If True returns the first person mesh, else returns the third
	*/
	USkeletalMeshComponent* GetSpecificPawnMesh(bool WantsFirstPerson) const;

protected:
	
	/** Fires a projectile. */
	void Fire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/** Player pressed targeting action */
	void OnStartTargeting();

	/** Player released targeting action */
	void OnStopTargeting();

	// Weapon Usage

	/** [server + local] change targeting state */
	void SetTargeting(bool bNewTargeting);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// Reading Data

protected:
	/** Current targeting state */
	UPROPERTY()
	uint8 bIsTargeting : 1;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<AFPSWeapon> WeaponClass;

	UPROPERTY()
	class AFPSWeapon* Weapon;

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1PComponent; }

};