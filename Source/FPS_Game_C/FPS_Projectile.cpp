// Fill out your copyright notice in the Description page of Project Settings.

#include "FPS_Game_C.h"
#include "FPS_Projectile.h"



// Sets default values
AFPS_Projectile::AFPS_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AFPS_Projectile::OnHit);

	CollisionComponent->InitSphereRadius(5.f);
	RootComponent = CollisionComponent;


	ParticleCom = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleCom"));
	ParticleCom->bAutoActivate = false;
	ParticleCom->bAutoDestroy = false;
	ParticleCom->SetupAttachment(RootComponent);


	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementCompoent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 2000.0f;
	ProjectileMovementComponent->MaxSpeed = 2000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	//ProjectileMovementComponent->bShouldBounce = true;
	//ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	

	InitialLifeSpan = 3.0f;

}

// Called when the game starts or when spawned
void AFPS_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPS_Projectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AFPS_Projectile::FireInDirection(const FVector & ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}
void AFPS_Projectile::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		UE_LOG(LogTemp, Log, TEXT("Projectile On Hit "));
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
		Destroy();
	}
}