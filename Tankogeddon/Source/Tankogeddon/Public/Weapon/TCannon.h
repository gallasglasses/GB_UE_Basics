// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStructs.h"
#include "TCannon.generated.h"

class UStaticMeshComponent;
class UArrowComponent;
class ATProjectile;
class UForceFeedbackEffect;
class UCameraShakeBase;
class UParticleSystemComponent;
class UAudioComponent;

USTRUCT(BlueprintType)
struct FAmmoData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		int32 Bullets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs", meta = (EditCondition = "!bInfinite"))
		int32 Clips;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		bool bInfinite;
};

UCLASS()
class TANKOGEDDON_API ATCannon : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
		UForceFeedbackEffect* ShootForceEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		ECannonType Type = ECannonType::FireProjectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "Type == ECannonType::FireProjectile", EditConditionHides), Category = "Turret Fire Parametrs")
		TSubclassOf<ATProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Effects")
		TSubclassOf<UCameraShakeBase> ShootShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "Type == ECannonType::FireProjectile", EditConditionHides), Category = "Turret Fire Parametrs")
		FAmmoData DefaultProjectileAmmoData{5, 1, false};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "Type == ECannonType::FireTrace", EditConditionHides), Category = "Turret Fire Parametrs")
		FAmmoData DefaultTraceAmmoData {1, 10, false};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		FAmmoData DefaultRifleAmmoData {10, 10, true};

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UParticleSystemComponent* ShootEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UAudioComponent* AudioEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "Type == ECannonType::FireProjectile", EditConditionHides), Category = "Turret Fire Parametrs")
		float FireRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		float FireRifleRate = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "Type == ECannonType::FireTrace", EditConditionHides), Category = "Turret Fire Parametrs")
		float FireRange = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		float FireDamage = 1.0f;

private:
	FTimerHandle ReloadTimerHandle;
	bool bReadyToFire = false;
	bool bRifleFire = false;
	bool bProjectileFire = false;
	bool bTraceFire = false;

public:
	ATCannon();

	UFUNCTION()
	ECannonType GetCannonType() const;

	void Fire();
	void StartRifleFire();
	void GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;
	void SetCannonType(ECannonType Type);
	void AddAmmo(ECannonType AmmoType, int32 AmmoAmount);
	void SetVisibility(bool bIsVisible);

	bool IsReadyToFire();
	bool IsBallistic() const;

	float FindElevationAngle(const FVector& InTarget);
	float FindDesiredBallisticTarget();
	float GetTargetRange(float FloorAbsoluteHeight) const;

	FVector GetCurrentBallisticTarget(float FloorAbsoluteHeight) const;

	FAmmoData CurrentRifleAmmo = DefaultRifleAmmoData;
	FAmmoData CurrentProjectileAmmo = DefaultProjectileAmmoData;
	FAmmoData CurrentTraceAmmo = DefaultTraceAmmoData;


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd);
	void RifleShot();
	void Shot();
	void Reload();
	void DecreaseAmmo(FAmmoData& CurrentDefaultAmmo);
	void ChangeClip(FAmmoData& CurrentDefaultAmmo);
	void LogAmmo(FAmmoData& CurrentDefaultAmmo);
	bool IsAmmoEmpty(FAmmoData CurrentDefaultAmmo) const;
	bool IsClipEmpty(FAmmoData CurrentDefaultAmmo) const;

};
