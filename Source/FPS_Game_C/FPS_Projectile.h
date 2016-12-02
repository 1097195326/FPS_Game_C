// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FPS_Projectile.generated.h"

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

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent * CollisionComponent;
	
	UPROPERTY(VisibleAnywhere, Category = Movement)
		UProjectileMovementComponent * ProjectileMovementComponent;
	
	void FireInDirection(const FVector & ShootDirection);

	UFUNCTION()
		void OnHit(UPrimitiveComponent * HitCompoent, class AActor * OtherActor, class UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit);

};
