// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStructs.h"
#include "TCannon.generated.h"

class UStaticMeshComponent;
class UArrowComponent;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		ECannonType Type = ECannonType::FireProjectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		FAmmoData DefaultAmmoData{10, 10, false};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		float FireRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		float FireRifleRate = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		float FireRange = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		float FireDamage = 1.0f;

private:
	FTimerHandle ReloadTimerHandle;
	bool bReadyToFire = false;
	FAmmoData CurrentAmmo;

public:
	ATCannon();

	void Fire();
	void StartRifleFire();
	bool IsReadyToFire();
	void GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd);
	void MakeShot();
	void Reload();
	void DecreaseAmmo();
	void ChangeClip();
	void LogAmmo();

	bool IsAmmoEmpty() const;
	bool IsClipEmpty() const;

};
