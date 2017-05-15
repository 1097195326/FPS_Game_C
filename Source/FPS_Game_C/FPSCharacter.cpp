// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Game_C.h"
#include "FPSCharacter.h"

#include "FPS_Projectile.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FPSCameraCom = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCameraCom"));
	FPSCameraCom->AttachTo(GetCapsuleComponent());
	FPSCameraCom->SetRelativeLocation(FVector(0.0f, 0.0f,10 + BaseEyeHeight));
	FPSCameraCom->bUsePawnControlRotation = true;

	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FPSMesh->AttachTo(FPSCameraCom);
	FPSMesh->SetOnlyOwnerSee(true);
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	WeaponMeshFP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshFP"));
	WeaponMeshFP->AttachTo(RootComponent);
	WeaponMeshFP->bCastDynamicShadow = false;
	WeaponMeshFP->CastShadow = false;
	WeaponMeshFP->SetOwnerNoSee(false);
	WeaponMeshFP->SetOnlyOwnerSee(true);

	WeaponMeshTP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshTP"));
	WeaponMeshTP->AttachTo(RootComponent);
	WeaponMeshTP->bCastDynamicShadow = false;
	WeaponMeshTP->CastShadow = false;
	WeaponMeshTP->SetOnlyOwnerSee(false);
	WeaponMeshTP->SetOwnerNoSee(true);

	GetMesh()->SetOwnerNoSee(true);

	Health = 100.0f;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("We are using FPSCharacter"));
	}
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	EquipWeapon();
}

// Called every frame
void AFPSCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::EndJump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSCharacter::StopFire);
}
float AFPSCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	/*UE_LOG(LogTemp, Log, TEXT("Character Take Damage : %f"),Health);*/
	if (Health < 0.0f)
	{
		return 0.0f;
	}
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f)
	{
		Health -= ActualDamage;
		if (Health <= 0.0f)
		{
			OnDealth();
		}
	}
	return ActualDamage;
}
void AFPSCharacter::OnDealth()
{
	Health = FMath::Min(0.0f, Health);
	DetachFromControllerPendingDestroy();
	StopAllAnimMontages();

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	float DeathAnimDuration = PlayAnimMontage(DeathAnim, 1.0f);
	UE_LOG(LogTemp, Log, TEXT("PlayAnimMontage : %f"), DeathAnimDuration);
	if (DeathAnimDuration > 0.f)
	{
		// Trigger ragdoll a little before the animation early so the character doesn't
		// blend back to its normal position.
		const float TriggerRagdollTime = DeathAnimDuration - 0.7f;

		// Enable blend physics so the bones are properly blending against the montage.
		GetMesh()->bBlendPhysics = true;

		UE_LOG(LogTemp, Log, TEXT("111111111111111"));
		// Use a local timer handle as we don't need to store it for later but we don't need to look for something to clear
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AFPSCharacter::SetRagdollPhysics, FMath::Max(0.1f, TriggerRagdollTime), false);
	}
	else
	{
		SetRagdollPhysics();
	}
	UE_LOG(LogTemp, Log, TEXT("333333333333"));
	// disable collisions on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}
void AFPSCharacter::SetRagdollPhysics()
{
	UE_LOG(LogTemp, Log, TEXT("2222222222"));
	bool bInRagdoll = false;

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}
	else if (!GetMesh() || !GetMesh()->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		// initialize physics/etc
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		bInRagdoll = true;
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	if (!bInRagdoll)
	{
		// hide and set short lifespan
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(10.0f);
	}
}
bool AFPSCharacter::IsDied()
{
	return Health <= 0.0f;
}
void AFPSCharacter::StopAllAnimMontages()
{
	USkeletalMeshComponent * UseMesh = GetUseMesh();
	if (UseMesh && UseMesh->AnimScriptInstance)
	{
		UseMesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}
void AFPSCharacter::MoveForward(float value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, value);
}
void AFPSCharacter::MoveRight(float value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}
void AFPSCharacter::StartJump()
{
	//bPressedJump = true;
	Jump();
}
void AFPSCharacter::EndJump()
{
	StopJumping();  
	//bPressedJump = false;
}
void AFPSCharacter::StartFire()
{
	if (ProjectileClass)
	{
		//FVector CameraLocation;
		//FRotator CameraRotation;
		//GetActorEyesViewPoint(CameraLocation, CameraRotation);

		USkeletalMeshComponent * UseWeaponMesh = GetUseWeaponMesh();
		FVector MuzzleLocation = UseWeaponMesh->GetSocketLocation(MuzzleAttachPoint);
		//FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
		//FRotator MuzzleRotation = CameraRotation;
		FRotator MuzzleRotation = GetControlRotation();

		//MuzzleRotation.Pitch += 10.0f;
		UWorld * world = GetWorld();

		if (world)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			AFPS_Projectile * Projectile = world->SpawnActor<AFPS_Projectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);

				//---play montage
				PlayAnimMontage(FP_FireMontage, 1.0f);
				
			}
			if (MuzzleFX)
			{
				 MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, GetUseWeaponMesh(), MuzzleAttachPoint);
			}

		}

	}

}
void AFPSCharacter::StopFire()
{
	if (MuzzlePSC != NULL)
	{
		MuzzlePSC->DeactivateSystem();
	}
}
bool AFPSCharacter::IsFirstPerson()
{
	return Controller && Controller->IsLocalPlayerController();
}
USkeletalMeshComponent * AFPSCharacter::GetUseMesh()
{
	return IsFirstPerson() ? FPSMesh : GetMesh();
}
USkeletalMeshComponent * AFPSCharacter::GetUseWeaponMesh()
{
	return IsFirstPerson() ? WeaponMeshFP : WeaponMeshTP;
}
float AFPSCharacter::PlayAnimMontage(UAnimMontage * Anim, float PlayRate)
{
	USkeletalMeshComponent * UseMesh = GetUseMesh();
	if (Anim && UseMesh && UseMesh->AnimScriptInstance)
	{
		//UE_LOG(LogTemp, Log, TEXT("Fire And Play Fire Montage"));
		return UseMesh->AnimScriptInstance->Montage_Play(Anim, PlayRate);
	}
	return 0.0f;
}
void AFPSCharacter::StopAnimMontage(UAnimMontage * Anim)
{
	USkeletalMeshComponent * UseMesh = GetUseMesh();
	if (Anim && UseMesh && UseMesh->AnimScriptInstance && UseMesh->AnimScriptInstance->Montage_IsPlaying(Anim))
	{
		UseMesh->AnimScriptInstance->Montage_Stop(Anim->BlendOut.GetBlendTime(), Anim);
	}
}
void AFPSCharacter::EquipWeapon()
{
	USkeletalMeshComponent * PawnFP = GetSpecificPawnMesh(true);
	USkeletalMeshComponent * PawnTP = GetSpecificPawnMesh(false);

	WeaponMeshFP->AttachToComponent(PawnFP, FAttachmentTransformRules::KeepRelativeTransform, WeaponAttachPoint);
	WeaponMeshTP->AttachToComponent(PawnTP, FAttachmentTransformRules::KeepRelativeTransform, WeaponAttachPoint);
	UE_LOG(LogTemp, Log, TEXT("character equip weapon"));

}
void AFPSCharacter::DetachWeapon()
{

}
USkeletalMeshComponent * AFPSCharacter::GetSpecificPawnMesh(bool WantFirstPerson)
{
	return WantFirstPerson ? FPSMesh : GetMesh();
}
FRotator AFPSCharacter::GetAnimOffsets() const
{
	const FVector AnimDirWS = GetBaseAimRotation().Vector();
	const FVector AnimDirLS = ActorToWorld().InverseTransformVectorNoScale(AnimDirWS);
	const FRotator AnimRotLS = AnimDirLS.Rotation();

	return AnimRotLS;
}