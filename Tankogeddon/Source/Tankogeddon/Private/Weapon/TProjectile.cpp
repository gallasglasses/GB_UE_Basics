// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TProjectile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "..\..\Tankogeddon.h"
#include "Components/Damageable.h"
#include "ActorPoolSubsystem.h"
#include "Damageable.h"

// Sets default values
ATProjectile::ATProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickInterval = 0.005f;

	//USceneComponent* Scene = CreateDefaultSubobject<USceneComponent>("Root");
	//RootComponent = Scene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Mesh->OnComponentHit.AddDynamic(this, &ATProjectile::OnProjectileHit);
	Mesh->SetHiddenInGame(true);
	Mesh->IgnoreActorWhenMoving(GetOwner(), true);
	RootComponent = Mesh;

}

void ATProjectile::Start()
{
	PrimaryActorTick.SetTickFunctionEnable(true);
	StartPosition = GetActorLocation();
	Mesh->SetHiddenInGame(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ATProjectile::Stop()
{
	PrimaryActorTick.SetTickFunctionEnable(false);
	Mesh->SetHiddenInGame(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UActorPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorPoolSubsystem>();
	if (Pool->IsActorInPool(this))
	{
		Pool->ReturnActor(this);
	}
	else
	{
		Destroy();
	}
}

void ATProjectile::Explode()
{
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
					//HitMesh->AddImpulse(ForceVector * ExplosionImpulse, NAME_None, true);
					HitMesh->AddForce(ForceVector * ExplosionImpulse, NAME_None, true);
					//HitMesh->AddForceAtLocation(ForceVector * ExplosionImpulse, SweepResult.ImpactPoint, NAME_None);
				}
			}
		}
	}

	Stop();
}

void ATProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NextPosition = GetActorLocation() + GetActorForwardVector() * MoveSpeed * DeltaTime;
	SetActorLocation(NextPosition, true);

	if (FVector::Dist(GetActorLocation(), StartPosition) > FireRange)
	{
		Stop();
	}
}

void ATProjectile::OnProjectileHit(UPrimitiveComponent* HittedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult)
{

	if (!bImpactExplosion)
	{
		if (OtherActor == GetInstigator())
		{
			Destroy();
			return;
		}

		if (OtherComp->IsSimulatingPhysics())
		{
			FVector Impulse = Mass * MoveSpeed * GetActorForwardVector();
			OtherComp->AddImpulseAtLocation(Impulse, SweepResult.ImpactPoint);
		}

		if (OtherActor && OtherComp && OtherComp->GetCollisionObjectType() == ECC_Destructible)
		{
			OtherActor->Destroy();
		}
		else if (IDamageable* Damageable = Cast<IDamageable>(OtherActor))
		{
			FDamageData DamageData;
			DamageData.DamageValue = DamageAmount;
			DamageData.Instigator = GetInstigator();
			DamageData.DamageMaker = this;
			Damageable->TakeDamage(DamageData);
		}
	}
	else 
	{
		Explode();

	}
	Stop();
}

