// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TProjectile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "..\..\Tankogeddon.h"
#include "Components/Damageable.h"
#include "ActorPoolSubsystem.h"
#include "Damageable.h"
#include "Kismet/GameplayStatics.h"

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
			{
				continue;
			}

			CheckDamageTaken(HitResult.GetActor());
			UPrimitiveComponent* HitMesh = Cast<UPrimitiveComponent>(HitActor->GetRootComponent());
			if (HitMesh)
			{
				CheckSimulatingPhysics(HitMesh, HitActor);
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

		CheckSimulatingPhysics(OtherComp, SweepResult);

		if (OtherActor && OtherComp && OtherComp->GetCollisionObjectType() == ECC_Destructible)
		{
			OtherActor->Destroy();
		}
		else
		{
			CheckDamageTaken(OtherActor);
		}
	}
	else 
	{
		Explode();

	}
	Stop();
}

void ATProjectile::CheckDamageTaken(AActor* DamageTakenActor)
{
	if (IDamageable* Damageable = Cast<IDamageable>(DamageTakenActor))
	{
		FDamageData DamageData;
		DamageData.DamageValue = DamageAmount;
		DamageData.Instigator = GetInstigator();
		DamageData.DamageMaker = this;
		Damageable->TakeDamage(DamageData);
	}
}

void ATProjectile::CheckSimulatingPhysics(UPrimitiveComponent* POtherComp, const FHitResult& PHitResult)
{
	if (POtherComp->IsSimulatingPhysics())
	{
		SpawnEffects();
		FVector Impulse = Mass * MoveSpeed * GetActorForwardVector();
		POtherComp->AddImpulseAtLocation(Impulse, PHitResult.ImpactPoint);
	}
}

void ATProjectile::CheckSimulatingPhysics(UPrimitiveComponent* PHitMesh, AActor* PHitActor)
{
	if (PHitMesh->IsSimulatingPhysics())
	{
		SpawnEffects();
		FVector ForceVector = PHitActor->GetActorLocation() - GetActorLocation();
		ForceVector.Normalize();
		PHitMesh->AddImpulse(ForceVector * ExplosionImpulse, NAME_None, true);
		//PHitMesh->AddForce(ForceVector * ExplosionImpulse, NAME_None, true);
		//PHitMesh->AddForceAtLocation(ForceVector * ExplosionImpulse, SweepResult.ImpactPoint, NAME_None);
	}
}

void ATProjectile::SpawnEffects()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, GetActorTransform().GetLocation(), GetActorTransform().GetRotation().Rotator(), FVector(3.0, 3.0, 3.0), true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitAudioEffect, GetActorLocation());
}
