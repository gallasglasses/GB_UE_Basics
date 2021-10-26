// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/PhysicsProjectile.h"
#include "PhysicsMovementComponent.h"

APhysicsProjectile::APhysicsProjectile()
{
	MovementComponent = CreateDefaultSubobject<UPhysicsMovementComponent>(TEXT("Movement Component"));
}

void APhysicsProjectile::Start()
{
	Super::Start();

	MovementComponent->Velocity = GetActorForwardVector() * MoveSpeed;
	MovementComponent->SetComponentTickEnabled(true);
}

void APhysicsProjectile::Stop()
{
	MovementComponent->Velocity = FVector::ZeroVector;
	MovementComponent->SetComponentTickEnabled(false);

	Super::Stop();
}

void APhysicsProjectile::Tick(float DeltaSeconds)
{
	if (GetActorLocation().Z < -10000.f)
	{
		Stop();
	}
}