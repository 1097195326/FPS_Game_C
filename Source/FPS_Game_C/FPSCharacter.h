// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

UCLASS()
class FPS_GAME_C_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector MuzzleOffset;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AFPS_Projectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere)
		UCameraComponent * FPSCameraCom;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent * FPSMesh;

	UPROPERTY(EditDefaultsOnly, Category = WeaponMesh)
		USkeletalMeshComponent * WeaponMeshFP;
	UPROPERTY(EditDefaultsOnly, Category = WeaponMesh)
		USkeletalMeshComponent * WeaponMeshTP;

	UPROPERTY(EditDefaultsOnly, Category = WeaponMesh)
		UParticleSystem * MuzzleFX;
	UPROPERTY()
		UParticleSystemComponent * MuzzlePSC;

	UPROPERTY(EditDefaultsOnly, Category = Anim)
		UAnimMontage * FP_FireMontage;
	UPROPERTY(EditDefaultsOnly, Category = Anim)
		UAnimMontage * TP_FireMontage;
	UPROPERTY(EditDefaultsOnly, Category = Text)
		FName WeaponAttachPoint;
	UPROPERTY(EditDefaultsOnly, Category = Text)
		FName MuzzleAttachPoint;
	//------------------------FUNCTION ----------------------
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void MoveForward(float value);
	UFUNCTION()
		void MoveRight(float value);
	UFUNCTION()
		void StartJump();
	UFUNCTION()
		void EndJump();
	UFUNCTION()
		void StartFire();
	UFUNCTION()
		void StopFire();

	bool IsFirstPerson();

	UFUNCTION(BlueprintCallable, Category = "Game")
	FRotator GetAnimOffsets() const;
	
	USkeletalMeshComponent * GetUseMesh();
	USkeletalMeshComponent * GetUseWeaponMesh();

	float PlayAnimMontage(UAnimMontage * anim, float PlayRate);
	void StopAnimMontage(UAnimMontage * anim);

	FName GetWeaponAttachPoint() { return WeaponAttachPoint; }

	void EquipWeapon();
	void DetachWeapon();

	USkeletalMeshComponent * GetSpecificPawnMesh(bool WantFirstPerson);
};
