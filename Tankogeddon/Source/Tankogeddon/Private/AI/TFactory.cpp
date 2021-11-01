// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TFactory.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "HealthComponent.h"
#include "Player/TPawn.h"
#include "../../Tankogeddon.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Level/MapLoader.h"

// Sets default values
ATFactory::ATFactory()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComp;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
	BuildingMesh->SetupAttachment(SceneComp);

	BuildingWallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Wall Mesh"));
	BuildingWallMesh->SetupAttachment(BuildingMesh);

	DestroyedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Destroyed Mesh"));
	DestroyedMesh->SetupAttachment(SceneComp);

	TankSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	TankSpawnPoint->SetupAttachment(SceneComp);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(SceneComp);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnDie.AddDynamic(this, &ATFactory::Die);
	HealthComponent->OnHealthChanged.AddDynamic(this, &ATFactory::DamageTaked);
}

void ATFactory::Die()
{
	if (MapLoader)
	{
		MapLoader->SetIsActivated(true);
	}
	bIsFactoryDestroyed = true;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathFactoryEffect, GetActorTransform().GetLocation(), GetActorTransform().GetRotation().Rotator(), FVector(4.0, 4.0, 4.0), true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathFactoryAudioEffect, GetActorLocation());
	BuildingMesh->SetHiddenInGame(true);
	BuildingWallMesh->SetHiddenInGame(true);
	DestroyedMesh->SetHiddenInGame(false);

	//Destroy();
}

void ATFactory::DamageTaked(float DamageValue)
{
	UE_LOG(LogTankogeddon, Log, TEXT("Factory %s taked damage:%f "), *GetName(), DamageValue);
}

void ATFactory::BeginPlay()
{
	Super::BeginPlay();

	//BuildingMesh->SetHiddenInGame(false);
	DestroyedMesh->SetHiddenInGame(true);
	
	GetWorld()->GetTimerManager().SetTimer(SpawnTankTimerHandle, this, &ATFactory::SpawnNewTank, SpawnTankRate, true, SpawnTankRate);
}

void ATFactory::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(SpawnTankTimerHandle);
}

void ATFactory::SpawnNewTank()
{
	if (!bIsFactoryDestroyed)
	{
		FTransform SpawnTransform(TankSpawnPoint->GetComponentRotation(), TankSpawnPoint->GetComponentLocation(), FVector(1.f));
		ATPawn* NewTank = GetWorld()->SpawnActorDeferred<ATPawn>(SpawnTankClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		NewTank->SetPatrollingPoints(TankWayPoints);

		NewTank->FinishSpawning(SpawnTransform);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnTankAudioEffect, GetActorLocation());
	}
}

void ATFactory::TakeDamage(const FDamageData& DamageData)
{
	if (!bIsFactoryDestroyed)
	{
		HealthComponent->TakeDamage(DamageData);
	}
}


