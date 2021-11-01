// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TFactory.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "HealthComponent.h"
#include "Player/TPawn.h"
#include "../../Tankogeddon.h"
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

	Destroy();
}

void ATFactory::DamageTaked(float DamageValue)
{
	UE_LOG(LogTankogeddon, Log, TEXT("Factory %s taked damage:%f "), *GetName(), DamageValue);
}

void ATFactory::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(SpawnTankTimerHandle, this, &ATFactory::SpawnNewTank, SpawnTankRate, true, SpawnTankRate);
}

void ATFactory::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(SpawnTankTimerHandle);
}

void ATFactory::SpawnNewTank()
{
	FTransform SpawnTransform(TankSpawnPoint->GetComponentRotation(), TankSpawnPoint->GetComponentLocation(), FVector(1.f));
	ATPawn* NewTank = GetWorld()->SpawnActorDeferred<ATPawn>(SpawnTankClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	NewTank->SetPatrollingPoints(TankWayPoints);
	
	NewTank->FinishSpawning(SpawnTransform);
}

void ATFactory::TakeDamage(const FDamageData& DamageData)
{
	HealthComponent->TakeDamage(DamageData);
}


