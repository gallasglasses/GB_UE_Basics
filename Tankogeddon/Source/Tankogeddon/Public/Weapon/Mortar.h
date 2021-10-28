// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Damageable.h"
#include "Components/Scoreable.h"
#include "Mortar.generated.h"

class UStaticMeshComponent;
class UStaticMeshComponent;
class UArrowComponent;
class UBoxComponent;
class UHealthComponent;
class ATCannon;
class APawn;
class UParticleSystem;
class USoundBase;
class ATAmmoPickup;

UCLASS()
class TANKOGEDDON_API AMortar : public AActor, public IDamageable, public IScoreable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMortar();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* MortarBodyMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* MortarMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonSetupPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UBoxComponent* HitCollider;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		UParticleSystem* DestructionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		USoundBase* DestructionAudioEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		UParticleSystem* DeathEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		USoundBase* DeathAudioEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		TSubclassOf<ATCannon> CannonClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonuses")
		TSubclassOf<ATAmmoPickup> LootBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float TargetingRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float TargetingSpeed = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float Accuracy = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float ScoresForKilling = 10.f;

	const FString BodyMeshPath = "StaticMesh'/Game/Meshes/SM_CSC_Body1.SM_CSC_Body1'";
	const FString TurretMeshPath = "StaticMesh'/Game/Meshes/SM_CSC_Gun7.SM_CSC_Gun7'";

	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	void Targeting();
	void RotateToPlayer();
	void Fire();
	bool IsPlayerInRange();
	bool CanFire();
	bool PlayerVisibilityControl();

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
		void OnHealthChanged(float Damage);

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
		void OnDie();

private:
	UPROPERTY()
		ATCannon* Cannon;

	UPROPERTY()
		APawn* PlayerPawn;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void TakeDamage(const FDamageData& DamageData) override;
	virtual int32 GetScoresForKilling() const override;
};
