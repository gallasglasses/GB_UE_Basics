// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TCannon.h"
//#include "..\..\Public\Weapon\TCannon.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include "TProjectile.h"

DEFINE_LOG_CATEGORY_STATIC(LogCannon, All, All);

void ATCannon::BeginPlay()
{
	Super::BeginPlay();

	bReadyToFire = true;
	CurrentRifleAmmo = DefaultRifleAmmoData;
	CurrentProjectileAmmo = DefaultProjectileAmmoData;
	CurrentTraceAmmo = DefaultTraceAmmoData;
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

	Shot();

}

void ATCannon::StartRifleFire()
{
	if (!bReadyToFire)
	{
		return;
	}
	if (IsAmmoEmpty(CurrentAmmo))
	{
		Reload();
		return;
	}
	bReadyToFire = false;

	RifleShot();
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ATCannon::RifleShot, 1.0f / FireRifleRate, true);
}

bool ATCannon::IsReadyToFire()
{
	return bReadyToFire;
}

bool ATCannon::AddAmmo(TSubclassOf<ATCannon> CannonClass, int32 AmmoAmount)
{
	FAmmoData DefaultAmmoData;
	if (bProjectileFire)
	{
		DefaultAmmoData.Bullets = DefaultProjectileAmmoData.Bullets;
	}
	if (bTraceFire)
	{
		DefaultAmmoData.Bullets = DefaultTraceAmmoData.Bullets;
	}
	if (bRifleFire)
	{
		DefaultAmmoData.Bullets = DefaultRifleAmmoData.Bullets;
	}
	if (CurrentAmmo.Bullets == DefaultAmmoData.Bullets || CurrentAmmo.bInfinite)
	{
		return false;
	}
	if (CurrentAmmo.Bullets + AmmoAmount > DefaultAmmoData.Bullets)
	{
		int32 CountAmmo = DefaultAmmoData.Bullets - CurrentAmmo.Bullets;
		
		if (CurrentAmmo.Clips < DefaultAmmoData.Clips)
		{
			CurrentAmmo.Clips++;
			CurrentAmmo.Bullets = AmmoAmount - CountAmmo;
		}
		else if(CurrentAmmo.Clips == DefaultAmmoData.Clips)
		{
			CurrentAmmo.Bullets = DefaultAmmoData.Bullets;
		}
	}
	else if(CurrentAmmo.Bullets + AmmoAmount == DefaultAmmoData.Bullets)
	{
		CurrentAmmo.Bullets = DefaultAmmoData.Bullets;
	}
	else
	{
		CurrentAmmo.Bullets += AmmoAmount;
	}
	return true;
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
	FCollisionQueryParams CollisionParams = FCollisionQueryParams(FName(TEXT("Fire")), true, this);;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.bReturnPhysicalMaterial = false;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}

void ATCannon::RifleShot()
{
	bRifleFire = true;
	CurrentAmmo = CurrentRifleAmmo;
	if (!GetWorld() || IsAmmoEmpty(CurrentAmmo))
	{
		Reload();
		return;
	}
	
	FVector TraceStart, TraceEnd;
	GetTraceData(TraceStart, TraceEnd);

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);

	ATProjectile* Projectile = GetWorld()->SpawnActor<ATProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
	if (Projectile)
	{
		Projectile->Start();
	}

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
	DecreaseAmmo(CurrentAmmo);
	bRifleFire = false;
}

void ATCannon::Shot()
{
	if (Type == ECannonType::FireProjectile)
	{
		CurrentAmmo = CurrentProjectileAmmo;
		if (IsAmmoEmpty(CurrentAmmo))
		{
			Reload();
			return;
		}
		bProjectileFire = true;
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Green, TEXT("Fire projectile"));

		ATProjectile* Projectile = GetWorld()->SpawnActor<ATProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
		if (Projectile)
		{
			Projectile->Start();
		}
		DecreaseAmmo(CurrentAmmo);
		bProjectileFire = false;
	}
	else if (Type == ECannonType::FireTrace)
	{
		CurrentAmmo = CurrentTraceAmmo;
		if (IsAmmoEmpty(CurrentAmmo))
		{
			Reload();
			return;
		}
		bTraceFire = true;
		FVector TraceStart, TraceEnd;
		GetTraceData(TraceStart, TraceEnd);

		FHitResult HitResult;
		MakeHit(HitResult, TraceStart, TraceEnd);

		if (HitResult.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), TraceStart, HitResult.ImpactPoint, FColor::Green, false, 1.0f, 0, 10.0f);
			if (HitResult.Actor.IsValid() && HitResult.Component.IsValid(), HitResult.Component->GetCollisionObjectType() == ECC_Destructible)
			{
				HitResult.Actor->Destroy();
			}
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Green, TEXT("Fire trace on object"));
		}
		else
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Yellow, false, 1.0f, 0, 10.0f);
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Yellow, TEXT("Fire trace"));
		}
		DecreaseAmmo(CurrentAmmo);
		bTraceFire = false;
	}
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ATCannon::Reload, 1.0f / FireRate, false);
}

void ATCannon::DecreaseAmmo(FAmmoData& CurrentAmmo)
{
	CurrentAmmo.Bullets--;
	LogAmmo(CurrentAmmo);

	if (IsClipEmpty(CurrentAmmo) && !IsAmmoEmpty(CurrentAmmo))
	{
		ChangeClip(CurrentAmmo);
	}
}

void ATCannon::ChangeClip(FAmmoData& CurrentAmmo)
{
	FAmmoData DefaultAmmoData;
	if (bProjectileFire)
	{
		DefaultAmmoData.Bullets = DefaultProjectileAmmoData.Bullets;
	}
	if (bTraceFire)
	{
		DefaultAmmoData.Bullets = DefaultTraceAmmoData.Bullets;
	}
	if (bRifleFire)
	{
		DefaultAmmoData.Bullets = DefaultRifleAmmoData.Bullets;
	}
	CurrentAmmo.Bullets = DefaultAmmoData.Bullets;
	if (!CurrentAmmo.bInfinite)
	{
		CurrentAmmo.Clips--;
	}
	UE_LOG(LogCannon, Display, TEXT("Change Clip"));
	Reload();
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

void ATCannon::LogAmmo(FAmmoData& CurrentAmmo)
{
	FString AmmoInfo = "Ammo: " + FString::FromInt(CurrentAmmo.Bullets) + " / ";
	AmmoInfo += CurrentAmmo.bInfinite ? "Infinite" : FString::FromInt(CurrentAmmo.Clips);
	UE_LOG(LogCannon, Display, TEXT("%s"), *AmmoInfo);

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, FString::Printf(TEXT("Rifle fire projectile %d / %d "), CurrentAmmo.Bullets, CurrentAmmo.Clips));
}

bool ATCannon::IsAmmoEmpty(FAmmoData& CurrentAmmo) const
{
	return !CurrentAmmo.bInfinite && CurrentAmmo.Clips == 0 && IsClipEmpty(CurrentAmmo);
}

bool ATCannon::IsClipEmpty(FAmmoData& CurrentAmmo) const
{
	return CurrentAmmo.Bullets == 0;
}