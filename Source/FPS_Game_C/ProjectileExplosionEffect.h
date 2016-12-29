// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectileExplosionEffect.generated.h"

USTRUCT()
struct FDecalData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Decal)
		UMaterial * DecalMaterial;
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		float DecalSize;
	UPROPERTY(EditDefaultsOnly, Category = Decal)
		float LifeSpan;
	FDecalData() :DecalSize(256.f), LifeSpan(10.f)
	{

	}
};

UCLASS()
class FPS_GAME_C_API AProjectileExplosionEffect : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		UParticleSystem * ExplosionFX;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Effect)
		UPointLightComponent * ExplosionLight;

	FName ExplosionLightComponentName;
public:	
	// Sets default values for this actor's properties
	AProjectileExplosionEffect();

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		float ExplosionLightFadeOut;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		USoundCue * ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = Effect)
		struct FDecalData Decal;

	UPROPERTY(BlueprintReadOnly, Category = Surface)
		FHitResult SurfaceHit;




	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
