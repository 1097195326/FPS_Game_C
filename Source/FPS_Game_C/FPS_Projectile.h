// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FPS_Projectile.generated.h"


USTRUCT()
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

		/** projectile class */
		/*UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AShooterProjectile> ProjectileClass;*/

	/** life time */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float ProjectileLife;

	/** damage at impact point */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		int32 ExplosionDamage;

	/** radius of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		float ExplosionRadius;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		TSubclassOf<UDamageType> DamageType;

	/** defaults */
	FProjectileWeaponData()
	{
		//ProjectileClass = NULL;
		ProjectileLife = 10.0f;
		ExplosionDamage = 100;
		ExplosionRadius = 300.0f;
		DamageType = UDamageType::StaticClass();
	}
};

UCLASS()
class FPS_GAME_C_API AFPS_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPS_Projectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	struct FProjectileWeaponData DamageConfig;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent * CollisionComponent;
	
	UPROPERTY(VisibleAnywhere, Category = Movement)
		UProjectileMovementComponent * ProjectileMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Particle)
		UParticleSystemComponent * ParticleCom;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		TSubclassOf<class AProjectileExplosionEffect> ExplosionTemplate;

	void FireInDirection(const FVector & ShootDirection);

	UFUNCTION()
		void OnHit(UPrimitiveComponent * HitCompoent, class AActor * OtherActor, class UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit);

};
