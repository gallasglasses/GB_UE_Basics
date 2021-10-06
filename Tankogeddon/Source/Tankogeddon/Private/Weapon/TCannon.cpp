// Fill out your copyright notice in the Description page of Project Settings.


//#include "Weapon/TCannon.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "..\..\Public\Weapon\TCannon.h"

ATCannon::ATCannon()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Scene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon mesh"));
	Mesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn point"));
	ProjectileSpawnPoint->SetupAttachment(Mesh);
}

void ATCannon::BeginPlay()
{
	Super::BeginPlay();
	
	bReadyToFire = true;
}

void ATCannon::Fire()
{
	if (!bReadyToFire)
	{
		return;
	}
	bReadyToFire = false;

	if (Type == ECannonType::FireProjectile)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Green, TEXT("Fire projectile"));
	}
	else if (Type == ECannonType::FireTrace)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Green, TEXT("Fire trace"));
	}

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ATCannon::Reload, 1.0f / FireRate, false);
}

bool ATCannon::IsReadyToFire()
{
	return bReadyToFire;
}

void ATCannon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

void ATCannon::Reload()
{
	bReadyToFire = true;
}
