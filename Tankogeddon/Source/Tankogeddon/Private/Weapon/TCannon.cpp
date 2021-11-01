// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TCannon.h"
//#include "..\..\Public\Weapon\TCannon.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogCannon, All, All);

void ATCannon::BeginPlay()
{
	Super::BeginPlay();

	bReadyToFire = true;
	CurrentAmmo = DefaultAmmoData;
}

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

void ATCannon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

void ATCannon::Fire()
{
	if (!GetWorld())
	{
		return;
	}
	if (!bReadyToFire)
	{
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Green, TEXT("Nooo fire projectile"));
		return;
	}
	bReadyToFire = false;

	if (Type == ECannonType::FireProjectile)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Green, TEXT("Fire projectile"));
	}
	else if (Type == ECannonType::FireTrace)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Green, TEXT("Fire trace"));
	}

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ATCannon::Reload, 2.0f / FireRate, false);
}

void ATCannon::StartRifleFire()
{
	if (!bReadyToFire)
	{
		return;
	}
	if (IsAmmoEmpty())
	{
		Reload();
		return;
	}
	bReadyToFire = false;

	MakeShot();
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ATCannon::MakeShot, 1.0f / FireRifleRate, true);
}

bool ATCannon::IsReadyToFire()
{
	return bReadyToFire;
}

void ATCannon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
	TraceStart = ProjectileSpawnPoint->GetComponentLocation();
	TraceEnd = TraceStart + FireRange * ProjectileSpawnPoint->GetForwardVector();
}

void ATCannon::Reload()
{
	bReadyToFire = true;
}

void ATCannon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
	if (!GetWorld())
	{
		return;
	}
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}

void ATCannon::MakeShot()
{
	if (!GetWorld() || IsAmmoEmpty())
	{
		Reload();
		return;
	}

	FVector TraceStart, TraceEnd;
	GetTraceData(TraceStart, TraceEnd);

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);

	if (HitResult.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), TraceStart, HitResult.ImpactPoint, FColor::Red, false, 1.0f, 0, 3.0f);
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 1.0f, 5, FColor::Red, false, 5.0f);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("Rifle fire projectile on object"));
	}
	else
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Yellow, false, 1.0f, 0, 3.0f);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Yellow, TEXT("Rifle fire projectile"));
	}
	DecreaseAmmo();
}

void ATCannon::DecreaseAmmo()
{
	CurrentAmmo.Bullets--;
	LogAmmo();

	if (IsClipEmpty() && !IsAmmoEmpty())
	{
		ChangeClip();
	}
}

void ATCannon::ChangeClip()
{
	CurrentAmmo.Bullets = DefaultAmmoData.Bullets;

	if (!CurrentAmmo.bInfinite)
	{
		CurrentAmmo.Clips--;
	}
	UE_LOG(LogCannon, Display, TEXT("Change Clip"));
	Reload();
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

void ATCannon::LogAmmo()
{
	FString AmmoInfo = "Ammo: " + FString::FromInt(CurrentAmmo.Bullets) + " / ";
	AmmoInfo += CurrentAmmo.bInfinite ? "Infinite" : FString::FromInt(CurrentAmmo.Clips);
	UE_LOG(LogCannon, Display, TEXT("%s"), *AmmoInfo);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, FString::Printf(TEXT("Rifle fire projectile %d / %d "), CurrentAmmo.Bullets, CurrentAmmo.Clips));
}

bool ATCannon::IsAmmoEmpty() const
{
	return !CurrentAmmo.bInfinite && CurrentAmmo.Clips == 0 && IsClipEmpty();
}

bool ATCannon::IsClipEmpty() const
{
	return CurrentAmmo.Bullets == 0;
}