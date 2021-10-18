// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TCannon.h"
//#include "..\..\Public\Weapon\TCannon.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include "TProjectile.h"
#include "ActorPoolSubsystem.h"
#include "Components/Damageable.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/ForceFeedbackEffect.h"

DEFINE_LOG_CATEGORY_STATIC(LogCannon, All, All);

void ATCannon::BeginPlay()
{
	Super::BeginPlay();

	bReadyToFire = true;/*
	CurrentRifleAmmo = DefaultRifleAmmoData;
	CurrentProjectileAmmo = DefaultProjectileAmmoData;
	CurrentTraceAmmo = DefaultTraceAmmoData;*/
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

	ShootEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Shoot Effect"));
	ShootEffect->SetupAttachment(ProjectileSpawnPoint);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Effect"));
	AudioEffect->SetupAttachment(ProjectileSpawnPoint);
}

void ATCannon::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

ECannonType ATCannon::GetCannonType() const
{
	return Type;
}

void ATCannon::Fire()
{
	if (bProjectileFire)
	{
		SetCannonType(ECannonType::FireProjectile);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Silver, TEXT("Fire FireProjectile"));
		bProjectileFire = !bProjectileFire;
	}
	else if (bTraceFire)
	{
		SetCannonType(ECannonType::FireTrace);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Silver, TEXT("Fire FireTrace"));
		bTraceFire = !bTraceFire;
	}
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

	if (GetCannonType() == ECannonType::FireTrace)
	{
		bTraceFire = !bTraceFire;
	}
	else //if (GetCannonType() == ECannonType::FireProjectile)
	{
		bProjectileFire = !bProjectileFire;
	}
	SetCannonType(ECannonType::FireRifle);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Silver, TEXT("Fire Rifle"));

	if (!bReadyToFire)
	{
		return;
	}
	if (IsAmmoEmpty(CurrentRifleAmmo))
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

void ATCannon::AddAmmo(ECannonType AmmoType, int32 AmmoAmount)
{
	int32 CountAmmo = 0,
		  CountClips = 0;
	if (AmmoType == ECannonType::FireProjectile)
	{
		CountClips = AmmoAmount / DefaultProjectileAmmoData.Bullets;
		CountAmmo = DefaultProjectileAmmoData.Bullets - CurrentProjectileAmmo.Bullets;

		CurrentProjectileAmmo.Clips = FMath::Clamp(CurrentProjectileAmmo.Clips + CountClips, 0, DefaultProjectileAmmoData.Clips);
		CurrentProjectileAmmo.Bullets = FMath::Clamp(CurrentProjectileAmmo.Bullets + AmmoAmount - CountAmmo, 0, DefaultProjectileAmmoData.Bullets);
	}
	else if (AmmoType == ECannonType::FireTrace)
	{
		CountClips = AmmoAmount / DefaultTraceAmmoData.Bullets;
		CountAmmo = DefaultTraceAmmoData.Bullets - CurrentTraceAmmo.Bullets;

		CurrentTraceAmmo.Clips = FMath::Clamp(CurrentTraceAmmo.Clips + CountClips, 0, DefaultTraceAmmoData.Clips);
		CurrentTraceAmmo.Bullets = FMath::Clamp(CurrentTraceAmmo.Bullets + AmmoAmount - CountAmmo, 0, DefaultTraceAmmoData.Bullets);
	}
	else if (AmmoType == ECannonType::FireRifle)
	{
		CountClips = AmmoAmount / DefaultRifleAmmoData.Bullets;
		CountAmmo = DefaultRifleAmmoData.Bullets - CurrentRifleAmmo.Bullets;

		CurrentRifleAmmo.Clips = FMath::Clamp(CurrentRifleAmmo.Clips + CountClips, 0, DefaultRifleAmmoData.Clips);
		CurrentRifleAmmo.Bullets = FMath::Clamp(CurrentRifleAmmo.Bullets + AmmoAmount - CountAmmo, 0, DefaultRifleAmmoData.Bullets);
	}
}

void ATCannon::SetVisibility(bool bIsVisible)
{
	Mesh->SetHiddenInGame(!bIsVisible);
}

void ATCannon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
	TraceStart = ProjectileSpawnPoint->GetComponentLocation();
	TraceEnd = TraceStart + FireRange * ProjectileSpawnPoint->GetForwardVector();
}

void ATCannon::SetCannonType(ECannonType CannonType)
{
	Type = CannonType;
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
	if (!GetWorld() || IsAmmoEmpty(CurrentRifleAmmo))
	{
		Reload();
		return;
	}
	
	FVector TraceStart, TraceEnd;
	GetTraceData(TraceStart, TraceEnd);

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);

	UActorPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorPoolSubsystem>();
	FTransform SpawnTransform(ProjectileSpawnPoint->GetComponentRotation(), ProjectileSpawnPoint->GetComponentLocation(), FVector::OneVector);
	ATProjectile* Projectile = Cast<ATProjectile>(Pool->RetreiveActor(ProjectileClass, SpawnTransform));
	if (Projectile)
	{
		Projectile->SetInstigator(GetInstigator());
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
	DecreaseAmmo(CurrentRifleAmmo);
	
}

void ATCannon::Shot()
{
	if (Type == ECannonType::FireProjectile)
	{
		if (IsAmmoEmpty(CurrentProjectileAmmo))
		{
			Reload();
			
			return;
		}
		ShootEffect->ActivateSystem();
		AudioEffect->Play();

		if (GetOwner() == GetWorld()->GetFirstPlayerController()->GetPawn())
		{
			if (ShootForceEffect)
			{
				FForceFeedbackParameters Params;
				Params.bLooping = false;
				Params.Tag = TEXT("ShootFFParams");
				GetWorld()->GetFirstPlayerController()->ClientPlayForceFeedback(ShootForceEffect);
			}

			if (ShootShake)
			{
				GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(ShootShake);
			}
		}
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Green, TEXT("Fire projectile"));

		UActorPoolSubsystem* Pool = GetWorld()->GetSubsystem<UActorPoolSubsystem>();
		FTransform SpawnTransform(ProjectileSpawnPoint->GetComponentRotation(), ProjectileSpawnPoint->GetComponentLocation(), FVector::OneVector);
		ATProjectile* Projectile = Cast<ATProjectile>(Pool->RetreiveActor(ProjectileClass, SpawnTransform));
		if (Projectile)
		{
			Projectile->SetInstigator(GetInstigator());
			Projectile->Start();
		}
		DecreaseAmmo(CurrentProjectileAmmo);
	}
	else if (Type == ECannonType::FireTrace)
	{
		if (IsAmmoEmpty(CurrentTraceAmmo))
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
			DrawDebugLine(GetWorld(), TraceStart, HitResult.ImpactPoint, FColor::Green, false, 1.0f, 0, 10.0f);
			if (HitResult.Actor.IsValid() && HitResult.Component.IsValid(), HitResult.Component->GetCollisionObjectType() == ECC_Destructible)
			{
				HitResult.Actor->Destroy();
			}
			else if (IDamageable* Damageable = Cast<IDamageable>(HitResult.Actor))
			{
				if (HitResult.Actor != GetInstigator())
				{
					FDamageData DamageData;
					DamageData.DamageValue = FireDamage;
					DamageData.Instigator = GetInstigator();
					DamageData.DamageMaker = this;
					Damageable->TakeDamage(DamageData);
				}
			}
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Green, TEXT("Fire trace on object"));
		}
		else
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Yellow, false, 1.0f, 0, 10.0f);
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Yellow, TEXT("Fire trace"));
		}
		DecreaseAmmo(CurrentTraceAmmo);
	}
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ATCannon::Reload, 1.0f / FireRate, false);
}

void ATCannon::DecreaseAmmo(FAmmoData& CurrentDefaultAmmo)
{
	CurrentDefaultAmmo.Bullets--;
	LogAmmo(CurrentDefaultAmmo);

	if (IsClipEmpty(CurrentDefaultAmmo) && !IsAmmoEmpty(CurrentDefaultAmmo))
	{
		ChangeClip(CurrentDefaultAmmo);
	}
}

void ATCannon::ChangeClip(FAmmoData& CurrentDefaultAmmo)
{
	FAmmoData DefaultAmmoData = {0,0,false};
	if (Type == ECannonType::FireProjectile)
	{
		DefaultAmmoData = DefaultProjectileAmmoData;
	}
	else if (Type == ECannonType::FireTrace)
	{
		DefaultAmmoData = DefaultTraceAmmoData;
	}
	else if (Type == ECannonType::FireRifle)
	{
		DefaultAmmoData = DefaultRifleAmmoData;
	}
	CurrentDefaultAmmo.Bullets = DefaultAmmoData.Bullets;
	if (!CurrentDefaultAmmo.bInfinite)
	{
		CurrentDefaultAmmo.Clips--;
	}
	UE_LOG(LogCannon, Display, TEXT("Change Clip"));
	Reload();
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
}

void ATCannon::LogAmmo(FAmmoData& CurrentDefaultAmmo)
{
	FString AmmoInfo = "Ammo: " + FString::FromInt(CurrentDefaultAmmo.Bullets) + " / ";
	AmmoInfo += CurrentDefaultAmmo.bInfinite ? "Infinite" : FString::FromInt(CurrentDefaultAmmo.Clips);
	UE_LOG(LogCannon, Display, TEXT("%s"), *AmmoInfo);
	if (CurrentDefaultAmmo.bInfinite)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, FString::Printf(TEXT("Projectile %d / Infinite "), CurrentDefaultAmmo.Bullets));
	}
	else 
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, FString::Printf(TEXT("Projectile %d / %d "), CurrentDefaultAmmo.Bullets, CurrentDefaultAmmo.Clips));
	}
}

bool ATCannon::IsAmmoEmpty(FAmmoData CurrentDefaultAmmo) const
{
	return !CurrentDefaultAmmo.bInfinite && CurrentDefaultAmmo.Clips == 0 && IsClipEmpty(CurrentDefaultAmmo);
}

bool ATCannon::IsClipEmpty(FAmmoData CurrentDefaultAmmo) const
{
	return CurrentDefaultAmmo.Bullets == 0;
}