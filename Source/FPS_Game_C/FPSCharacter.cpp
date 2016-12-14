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

	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCameraComponent"));
	FPSCameraComponent->AttachTo(GetCapsuleComponent());
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	FPSCameraComponent->bUsePawnControlRotation = true;

	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FPSMesh->AttachTo(FPSCameraComponent);
	FPSMesh->SetOnlyOwnerSee(true);
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->AttachTo(RootComponent);
	WeaponMesh->bCastDynamicShadow = false;
	WeaponMesh->CastShadow = false;

	GetMesh()->SetOwnerNoSee(true);

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
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
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
void AFPSCharacter::Fire()
{
	if (ProjectileClass)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
		FRotator MuzzleRotation = CameraRotation;

		MuzzleRotation.Pitch += 10.0f;
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

		}

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
float AFPSCharacter::PlayAnimMontage(UAnimMontage * Anim, float PlayRate)
{
	USkeletalMeshComponent * UseMesh = GetUseMesh();
	if (Anim && UseMesh && UseMesh->AnimScriptInstance)
	{
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

	WeaponMesh->AttachToComponent(PawnFP, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	//WeaponMesh->AttachToComponent(PawnTP, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	UE_LOG(LogTemp, Log, TEXT("character equip weapon"));

}
void AFPSCharacter::DetachWeapon()
{

}
USkeletalMeshComponent * AFPSCharacter::GetSpecificPawnMesh(bool WantFirstPerson)
{
	return WantFirstPerson ? FPSMesh : GetMesh();
}