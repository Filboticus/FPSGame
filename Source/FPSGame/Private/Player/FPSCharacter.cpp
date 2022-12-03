// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Player/FPSCharacter.h"
#include "Player/FPSCharacterMovement.h"
#include "FPSProjectile.h"
#include "Player/FPSPlayerCameraManager.h"
#include "Player/FPSPlayerController.h"
#include "FPSWeapon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "SoundNodeLocalPlayer.h"
#include "AudioThread.h"


AFPSCharacter::AFPSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFPSCharacterMovement>(AFPSCharacter::CharacterMovementComponentName))
{
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1PComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh1PComponent->SetupAttachment(GetCapsuleComponent());
	Mesh1PComponent->bOnlyOwnerSee = true;
	Mesh1PComponent->bOwnerNoSee = false;
	Mesh1PComponent->bCastDynamicShadow = false;
	Mesh1PComponent->bReceivesDecals = false;
	Mesh1PComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1PComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	Mesh1PComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1PComponent->SetCollisionObjectType(ECC_Pawn);
	Mesh1PComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel2, ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// Create a gun mesh component
	/** GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	GunMeshComponent->CastShadow = false;
	GunMeshComponent->SetupAttachment(Mesh1PComponent, "GripPoint");
	GunMeshComponent->bOnlyOwnerSee = false;
	GunMeshComponent->bOnlyOwnerSee = true;
	GunMeshComponent->bReceivesDecals = false; */

	TargetingSpeedModifier = 0.5f;
	bIsTargeting = false;
	RunningSpeedModifier = 2.0f;
	bWantsToRun = false;
}


void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSCharacter::ReloadWeapon);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Targeting", IE_Pressed, this, &AFPSCharacter::OnStartTargeting);
	PlayerInputComponent->BindAction("Targeting", IE_Released, this, &AFPSCharacter::OnStopTargeting);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AFPSCharacter::OnStartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AFPSCharacter::OnStopRunning);

}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	Weapon = GetWorld()->SpawnActor<AFPSWeapon>(WeaponClass);
	Weapon->AttachToComponent(Mesh1PComponent, FAttachmentTransformRules::KeepRelativeTransform, TEXT("GripPoint"));
	Weapon->SetOwner(this);

	Health = GetMaxHealth();
	UE_LOG(LogTemp, Warning, TEXT("Health = %f"), Health);
}

void AFPSCharacter::Fire()
{
	Weapon->FireWeapon();

	// try and play a firing animation if specified
	if (FireAnimation)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, "Arms", 0.0f);
		}
	}
}

float AFPSCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{
		return 0.0f;
	}
	
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f)
	{
		Health -= ActualDamage;
		if (Health <= 0.0f)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}

	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

	return ActualDamage;

	//float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//DamageToApply = FMath::Min(Health, DamageToApply);
	//Health -= DamageToApply;
	//UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
	
	//return DamageToApply;
}

void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}


void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}


void AFPSCharacter::OnStartRunning()
{
	AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(Controller);
	if (!ensure(PlayerController != nullptr)) return;
	
	if (PlayerController)
	{
		if (IsTargeting())
		{
			SetTargeting(false);
		}
		SetRunning(true, false);
		UE_LOG(LogTemp, Warning, TEXT("OnStartRunning"));
	}
}

void AFPSCharacter::OnStopRunning()
{
	SetRunning(false, false);
	UE_LOG(LogTemp, Warning, TEXT("OnStopRunning"));
}

void AFPSCharacter::OnStartTargeting()
{
	SetTargeting(true);
}

void AFPSCharacter::OnStopTargeting()
{
	SetTargeting(false);
}

void AFPSCharacter::OnCameraUpdate(const FVector& CameraLocaiton, const FRotator& CameraRotation)
{
	USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("CharacterMesh")));
	const FMatrix DefMeshLS = FRotationTranslationMatrix(DefMesh1P->GetRelativeRotation(), DefMesh1P->GetRelativeLocation());
	const FMatrix LocaltoWorld = ActorToWorld().ToMatrixWithScale();

	// Mesh Rotating code expect uniform scale in LocalToWorld matrix

	const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
	const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocaiton) * LocaltoWorld.Inverse();
	const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
	const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.Inverse();
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	Mesh1PComponent->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
}

// Weapon Usage

void AFPSCharacter::ReloadWeapon()
{
}

void AFPSCharacter::SetTargeting(bool bNewTargeting)
{
	bIsTargeting = bNewTargeting;
}

float AFPSCharacter::GetTargetingSpeedModifier() const
{
	return TargetingSpeedModifier;
}

bool AFPSCharacter::IsTargeting() const
{
	return bIsTargeting;
}

float AFPSCharacter::GetRunningSpeedModifier() const
{
	return RunningSpeedModifier;
}

bool AFPSCharacter::IsRunning() const
{
	if (!GetCharacterMovement())
	{
		return false;
	}
	return bWantsToRun && !GetVelocity().IsZero() && (GetVelocity().GetSafeNormal2D() | GetActorForwardVector()) > -0.1;
}

USkeletalMeshComponent* AFPSCharacter::GetSpecificPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? Mesh1PComponent : GetMesh();
}

int32 AFPSCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<AFPSCharacter>()->Health;
}

// Movement

void AFPSCharacter::SetRunning(bool bNewRunning, bool bToggle)
{
	bWantsToRun = bNewRunning;
	
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerSetRunning(bNewRunning, bToggle);
	}
}

bool AFPSCharacter::ServerSetRunning_Validate(bool bNewRunning, bool bToogle)
{
	return true;
}

void AFPSCharacter::ServerSetRunning_Implementation(bool bNewRunning, bool bToggle)
{
	SetRunning(bNewRunning, bToggle);
}

// Damage & Death

void AFPSCharacter::Suicide()
{
	KilledBy(this);
}

void AFPSCharacter::KilledBy(class APawn* EventInstigator)
{
	if (GetLocalRole() == ROLE_Authority && !bIsDying)
	{
		AController* Killer = NULL;
		if (EventInstigator != NULL)
		{
			Killer = EventInstigator->Controller;
			LastHitBy = NULL;
		}

		Die(Health, FDamageEvent(UDamageType::StaticClass()), Killer, NULL);
	}
}

bool AFPSCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying								// already dying
		|| IsPendingKill()						// already destroyed
		|| GetLocalRole() != ROLE_Authority)	// not authority 
	{
		return false;
	}
		
	return true;
	
	//* ToDo: Setup GameMode Add to the above if statemet
	//	|| GetWorld()->GetAuthGameMode<AFPSGameMode>() == NULL
	//	|| GetWorld()->GetAuthGameMode<AFPSGamemMode>()->GetMatchState() == MatchState::LeavingMap) // level transition occuring */
}

bool AFPSCharacter::Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Min(0.0f, Health);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	//GetWorld()->GetAuthGameMode<AFPSGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	NetUpdateFrequency = GetDefault<AFPSCharacter>()->NetUpdateFrequency;
	GetCharacterMovement()->ForceReplicationUpdate();

	//OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);

	return true;
}

void AFPSCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	Die(Health, FDamageEvent(dmgType.GetClass()), NULL, NULL);
}

/* void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{

} */

 void AFPSCharacter::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	SetReplicatingMovement(false);
	TearOff();
	bIsDying = true;

	if (GetLocalRole() == ROLE_Authority)
	{
		//ReplicatedHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);
	}

	// cannot use IsLocallyControlled here, because even local client's controller may be NULL here
	if (GetNetMode() != NM_DedicatedServer && DeathSound && Mesh1PComponent && Mesh1PComponent->IsVisible())
	{
		//UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	// remove all weapons
	//DestoryInventory();

	// Switch back to 3rd person view
	//UpdatePawnMeshes();

	//DetachFromControllerPendingDestory();
	//StopAllAnimMontages();

	/* if (LowHealthWarningPlayer && LowHealthWarningPlayer->IsPlaying())
	{
		LowHealthWarningPlayer->Stop();
	} */

	/*if (RunLoopAC)
	{
		RunLoopAC->Stop();
	} */

	// Death animation
	float DeathAnimDuration = PlayAnimMontage(DeathAnim);

	// Ragdoll
	if (DeathAnimDuration > 0.0f)
	{
		// Trigger ragdoll a little before the animation early so the character doesn't
		// blend back to its normal position.
		const float TriggerRagdollTime = DeathAnimDuration - 0.7f;

		// Enable blend physics so the bones are properly blending against the montage.
		GetMesh()->bBlendPhysics = true;

		// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
		FTimerHandle TimerHandle;
		//GetWorldTimerPlayerCameraManagerClass().SetTImer(TimerHandle, this, &AFPSCharacter::SetRagdollPhysics, FMath::Max(0.1f, TriggerRagdollTime), false);
	}
	else
	{
		//SetRagdollPhysics();
	}

	// Disable collisions on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
} 

 void AFPSCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	 if (GetLocalRole() == ROLE_Authority)
	 {
		 // To-Do: Create ReplicateHit Function 
		 //ReplicateHit(DamageTake, DamageEvent, PawnInstigator, DamageCauser, false);
	 }
}

