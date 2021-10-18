// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Turret.h"
#include "Player/TPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/ArrowComponent.h"
#include "Weapon/TCannon.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/NoExportTypes.h"
#include "../../Tankogeddon.h"
#include "GameStructs.h"
#include "HealthComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.005f;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret body"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret turret"));
	TurretMesh->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(BodyMesh);

	UStaticMesh* TurretMeshTemp = LoadObject<UStaticMesh>(this, *TurretMeshPath);
	if (TurretMeshTemp)
	{
		TurretMesh->SetStaticMesh(TurretMeshTemp);
	}

	UStaticMesh* BodyMeshTemp = LoadObject<UStaticMesh>(this, *BodyMeshPath);
	if (BodyMeshTemp)
	{
		BodyMesh->SetStaticMesh(BodyMeshTemp);
	}

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ATurret::OnHealthChanged);
	HealthComponent->OnDie.AddDynamic(this, &ATurret::OnDie);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Cannon = GetWorld()->SpawnActor<ATCannon>(CannonClass, Params);
	Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

}

void ATurret::Destroyed()
{
	Super::Destroyed();

	if (Cannon)
	{
		Cannon->Destroy();
	}
}


void ATurret::Targeting()
{
	if (IsPlayerInRange())
	{
		RotateToPlayer();
	}

	if (CanFire() && Cannon && Cannon->IsReadyToFire())
	{
		Fire();
	}
}

void ATurret::RotateToPlayer()
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
	FRotator CurrRotation = TurretMesh->GetComponentRotation();
	TargetRotation.Pitch = CurrRotation.Pitch;
	TargetRotation.Roll = CurrRotation.Roll;
	TurretMesh->SetWorldRotation(FMath::RInterpConstantTo(CurrRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), TargetingSpeed));
}

bool ATurret::IsPlayerInRange()
{
	return FVector::DistSquared(PlayerPawn->GetActorLocation(), GetActorLocation()) <= FMath::Square(TargetingRange);
}

bool ATurret::CanFire()
{
	if (!PlayerVisibilityControl())
	{
		return false;
	}
	FVector TargetingDir = TurretMesh->GetForwardVector();
	FVector DirToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
	DirToPlayer.Normalize();
	float AimAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDir, DirToPlayer)));
	return AimAngle <= Accuracy;
}

void ATurret::Fire()
{
	if (Cannon)
	{
		Cannon->Fire();
	}
}

void ATurret::OnHealthChanged_Implementation(float Damage)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, FString::Printf(TEXT("Turret %s taked damage:%f "), *GetName(), Damage));
}

void ATurret::OnDie_Implementation()
{
	Destroy();
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerPawn)
	{
		Targeting();
	}

}

void ATurret::TakeDamage(const FDamageData& DamageData)
{
	HealthComponent->TakeDamage(DamageData);
}

int32 ATurret::GetScoresForKilling() const
{
	return ScoresForKilling;
}

bool ATurret::PlayerVisibilityControl()
{
	FHitResult HitResult;
	FVector TraceStart = CannonSetupPoint->GetComponentLocation();
	FVector TraceEnd = PlayerPawn->GetActorLocation();
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Turret Vission Trace")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
	{
		if (HitResult.Actor.Get())
		{
			DrawDebugLine(GetWorld(), TraceStart, HitResult.Location, FColor::Purple, false, 0.1f, 0, 5);
			return HitResult.Actor.Get() == PlayerPawn;
		}
	}
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Purple, false, 0.1f, 0, 5);
	return false;
}