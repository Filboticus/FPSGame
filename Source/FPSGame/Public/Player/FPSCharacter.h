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
class USoundCue;


UCLASS()
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFPSCharacter(const FObjectInitializer& ObjectInitializer);

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AFPSProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
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

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	float GetTargetingSpeedModifier() const;

	/** Get targeting state */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	bool IsTargeting() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	float GetRunningSpeedModifier() const;

	/** get running state */
	UFUNCTION(BlueprintCallable, Category = Pawn)
	bool IsRunning() const;

	/** get max health */
	int32 GetMaxHealth() const;

	/** get death state*/
	UFUNCTION(BlueprintPure) //BlueprintPure does not have an execusion pin
	bool IsDead() const;

	/** Get either first or third person mesh.
	*
	* @param	WantFirstPerson If True returns the first person mesh, else returns the third
	*/
	USkeletalMeshComponent* GetSpecificPawnMesh(bool WantsFirstPerson) const;

	/** Override TakeDage from Actor.h */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private: 
	/** Pawn mesh: 1st person view  */
	UPROPERTY(VisibleDefaultsOnly, Category="Mesh")
	USkeletalMeshComponent* Mesh1PComponent;


	/** Fires a projectile. */
	void Fire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/** Player pressed run action */
	void OnStartRunning();

	/** Player released run action */
	void OnStopRunning();

	/** [server + local] change running state */
	void SetRunning(bool bNewRunning, bool bToggle);

	/** upate running state */
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetRunning(bool bNewRunning, bool bToggle);

	/** Player pressed targeting action */
	void OnStartTargeting();

	/** Player released targeting action */
	void OnStopTargeting();

	// Weapon Usage

	/** Reloads the current weapon */
	void ReloadWeapon();

	/** [server + local] change targeting state */
	void SetTargeting(bool bNewTargeting);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// Reading Data

protected:
	/** modifier for max movement speed */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	float TargetingSpeedModifier;
	
	/** Current targeting state */
	UPROPERTY() //(Transient, Replicated)
	uint8 bIsTargeting : 1;

	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	float RunningSpeedModifier;

	/** current running state */
	UPROPERTY() //(Transient, Replicated)
	uint8 bWantsToRun : 1;

	/** animation played on death */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* DeathAnim;

	/** sound played on death, local player only */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	USoundCue* DeathSound;



	// Damage & Death

public:
	/** Identifies if pawn is in its dying state */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	uint32 bIsDying : 1;

	/** Current health of the Pawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	/** Take Damage, handle Death */
	//virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	/** Pawn Suicide */
	virtual void Suicide();

	/** Kill this pawn */
	virtual void KilledBy(class APawn* EventInstigator);

	/** Returns true if the pawn can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	/**
	* Kills pawn. Server/authority only.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamagerCauser - the Actor that directly caused the damge (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	// Die when we fall out of the world
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	/** Called on the actor right before replication occurs */
	//virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

protected:
	/** Notificaiton when killed, for both the server and client */
	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);

	/** Play effects on hit */
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	/** Switch to ragdoll */
	//void SetRagdollPhysics();

	/** Sets up the replication for taking a hit */
	//void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCuaser, bool bkilled);

	/** Play hit or death on client */
	//UFUNCTION()
	//void OnRep_LastTakeHitInfo();




private:

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<AFPSWeapon> WeaponClass;

	UPROPERTY()
	class AFPSWeapon* Weapon;

protected:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1PComponent; }

};