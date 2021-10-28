// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Mortar.h"
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
#include "Kismet/GameplayStatics.h"
#include "Pickups/TAmmoPickup.h"

// Sets default values
AMortar::AMortar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.005f;

	MortarBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mortar body"));
	RootComponent = MortarBodyMesh;

	MortarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mortar turret"));
	MortarMesh->AttachToComponent(MortarBodyMesh, FAttachmentTransformRules::KeepRelativeTransform);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	CannonSetupPoint->AttachToComponent(MortarMesh, FAttachmentTransformRules::KeepRelativeTransform);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(MortarBodyMesh);

	UStaticMesh* TurretMeshTemp = LoadObject<UStaticMesh>(this, *TurretMeshPath);
	if (TurretMeshTemp)
	{
		MortarMesh->SetStaticMesh(TurretMeshTemp);
	}

	UStaticMesh* BodyMeshTemp = LoadObject<UStaticMesh>(this, *BodyMeshPath);
	if (BodyMeshTemp)
	{
		MortarBodyMesh->SetStaticMesh(BodyMeshTemp);
	}

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &AMortar::OnHealthChanged);
	HealthComponent->OnDie.AddDynamic(this, &AMortar::OnDie);
}

// Called when the game starts or when spawned
void AMortar::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters Params;
	Params.Owner = this;
	Cannon = GetWorld()->SpawnActor<ATCannon>(CannonClass, Params);
	Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
}

void AMortar::Destroyed()
{
	Super::Destroyed();

	if (Cannon)
	{
		Cannon->Destroy();
	}
}


void AMortar::Targeting()
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

void AMortar::RotateToPlayer()
{
	FRotator TargetBodyRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
	FRotator CurrBodyRotation = MortarBodyMesh->GetComponentRotation();
	TargetBodyRotation.Pitch = CurrBodyRotation.Pitch;
	TargetBodyRotation.Roll = CurrBodyRotation.Roll;
	MortarBodyMesh->SetWorldRotation(FMath::RInterpConstantTo(CurrBodyRotation, TargetBodyRotation, GetWorld()->GetDeltaSeconds(), TargetingSpeed));
	float DistanceBetween = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Black, FString::Printf(TEXT("DistanceBetween %f"), DistanceBetween));
	float HeightCannonSetupPoint = CannonSetupPoint->GetComponentLocation().Z;
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Black, FString::Printf(TEXT("HeightCannonSetupPoint %f"), HeightCannonSetupPoint));
	FRotator TargetRotation;
	FRotator CurrRotation = MortarMesh->GetComponentRotation();
	TargetRotation.Yaw = CurrRotation.Yaw;
	TargetRotation.Roll = CurrRotation.Roll;
	TargetRotation.Pitch = FMath::RadiansToDegrees(0.5f * (FMath::Acos(((980.0f * FMath::Square(DistanceBetween) / FMath::Square(1000.f)) - HeightCannonSetupPoint) / FMath::Sqrt(FMath::Square(HeightCannonSetupPoint) + FMath::Square(DistanceBetween))) + FMath::Atan(DistanceBetween / HeightCannonSetupPoint)));
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Black, FString::Printf(TEXT("TargetRotation.Pitch %f"), TargetRotation.Pitch));
	MortarMesh->SetWorldRotation(FMath::RInterpConstantTo(CurrRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), TargetingSpeed));	
}

bool AMortar::IsPlayerInRange()
{
	return FVector::DistSquared(PlayerPawn->GetActorLocation(), GetActorLocation()) <= FMath::Square(TargetingRange);
}

bool AMortar::CanFire()
{
	if (!PlayerVisibilityControl())
	{
		return false;
	}
	/*FVector TargetingDir = MortarMesh->GetForwardVector();
	FVector DirToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
	DirToPlayer.Normalize();
	float AimAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDir, DirToPlayer)));
	return AimAngle <= Accuracy;*/
	return true;
}

void AMortar::Fire()
{
	if (Cannon)
	{
		Cannon->Fire();
	}
}

void AMortar::OnHealthChanged_Implementation(float Damage)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, FString::Printf(TEXT("Turret %s taked damage:%f "), *GetName(), Damage));
}

void AMortar::OnDie_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathEffect, GetActorTransform().GetLocation(), GetActorTransform().GetRotation().Rotator(), FVector(3.0, 3.0, 3.0), true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathAudioEffect, GetActorLocation());

	if (LootBox)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoFail = true;
		GetWorld()->SpawnActor<ATAmmoPickup>(LootBox, GetActorTransform(), SpawnParams);
	}

	Destroy();
}

void AMortar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerPawn)
	{
		Targeting();
	}

}

void AMortar::TakeDamage(const FDamageData& DamageData)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestructionEffect, GetActorTransform().GetLocation(), GetActorTransform().GetRotation().Rotator(), FVector(3.0, 3.0, 3.0), true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestructionAudioEffect, GetActorLocation());
	HealthComponent->TakeDamage(DamageData);
}

int32 AMortar::GetScoresForKilling() const
{
	return ScoresForKilling;
}

bool AMortar::PlayerVisibilityControl()
{
	FHitResult HitResult;
	FVector TraceStart = CannonSetupPoint->GetComponentLocation();
	FVector TraceEnd = PlayerPawn->GetActorLocation();
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Turret Vission Trace")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
	{
		DrawDebugLine(GetWorld(), TraceStart, HitResult.Location, FColor::Purple, false, 0.1f, 0, 5);
		if (HitResult.Actor.Get())
		{
			return HitResult.Actor.Get() == PlayerPawn;
		}
	}
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Purple, false, 0.1f, 0, 5);
	return false;
}
