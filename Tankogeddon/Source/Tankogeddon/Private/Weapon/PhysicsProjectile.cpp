// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/PhysicsProjectile.h"
#include "PhysicsMovementComponent.h"
#include "Damageable.h"

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

void APhysicsProjectile::OnProjectileHit(UPrimitiveComponent* HittedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult)
{
	Super::OnProjectileHit(HittedComp, OtherActor, OtherComp, NormalImpulse, SweepResult);
	FVector StartPos = GetActorLocation();
	FVector EndPos = StartPos + FVector(0.1f);

	FCollisionShape Shape = FCollisionShape::MakeSphere(ExplosionRange);
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;
	Params.TraceTag = "Explode Trace";
	TArray<FHitResult> AttackHit;

	FQuat Rotation = FQuat::Identity;

	GetWorld()->DebugDrawTraceTag = "Explode Trace";

	bool bSweepResult = GetWorld()->SweepMultiByChannel
	(
		AttackHit,
		StartPos,
		EndPos,
		Rotation,
		ECollisionChannel::ECC_Visibility,
		Shape,
		Params
	);

	if (bSweepResult)
	{
		for (FHitResult HitResult : AttackHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if (!HitActor)
				continue;

			IDamageable* DamageableActor = Cast<IDamageable>(HitActor);
			if (DamageableActor)
			{
				FDamageData DamageData;
				DamageData.DamageValue = DamageAmount;
				DamageData.Instigator = GetOwner();
				DamageData.DamageMaker = this;

				DamageableActor->TakeDamage(DamageData);
			}

			UPrimitiveComponent* HitMesh = Cast<UPrimitiveComponent>(HitActor->GetRootComponent());
			if (HitMesh)
			{
				if (HitMesh->IsSimulatingPhysics())
				{
					FVector ForceVector = HitActor->GetActorLocation() - GetActorLocation();
					ForceVector.Normalize();
					HitMesh->AddImpulse(ForceVector * ExplosionImpulse, NAME_None, true);
				}
			}
		}
	}

	Stop();
}