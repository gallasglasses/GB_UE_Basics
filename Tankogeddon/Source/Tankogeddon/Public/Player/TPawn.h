
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Damageable.h"
#include "Components/Scoreable.h"
#include "TPawn.generated.h"

//DECLARE_LOG_CATEGORY_EXTERN(LogPawn, All, All);

class UStaticMeshComponent;
class UStaticMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class UArrowComponent;
class UBoxComponent;
class UHealthComponent;
class ATCannon;
class UParticleSystem;
class USoundBase;
class ATAmmoPickup;
class ATargetPoint;

UCLASS()
class TANKOGEDDON_API ATPawn : public APawn, public IDamageable, public IScoreable
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* S_TBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* S_TTurret;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonSpawnPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UBoxComponent* HitCollider;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		UParticleSystem* DeathEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		USoundBase* DeathAudioEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USoundBase* HitAudioEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float MoveSpeed = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float RotationSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float MovementSmoothness = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float RotationSmoothness = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret Fire Parametrs")
		float ScoresForKilling = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params")
		float MovementAccuracy = 50.0f;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret")
		float TurretRotationSmoothness = 0.5f;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Move params", Meta = (MakeEditWidget = true))
		TArray<ATargetPoint*> PatrollingPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret")
		TSubclassOf<ATCannon> DefaultCannonClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonuses")
		TSubclassOf<ATAmmoPickup> LootBox;

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
		void OnHealthChanged(float Damage);

	UFUNCTION(BlueprintNativeEvent, Category = "Health")
		void OnDie();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void TakeDamage(const FDamageData& DamageData) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void MoveForward(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Movement")
		void RotateRight(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Turret")
		void SetTurretTargetPosition(const FVector& TargetPosition);

	UFUNCTION(BlueprintCallable, Category = "Turret")
		void SetTurretRotationAxis(float AxisValue);

	UFUNCTION(BlueprintCallable, Category = "Turret")
		void Fire();
		
	UFUNCTION(BlueprintCallable, Category = "Turret")
		void StartRifleFire();

	UFUNCTION(BlueprintCallable, Category = "Turret")
		void SetupCannon(TSubclassOf<ATCannon> InCannonClass);

	UFUNCTION(BlueprintCallable, Category = "Turret")
		void NextWeapon();

	UFUNCTION(BlueprintCallable, Category = "AI|Move params")
		void SetPatrollingPoints(const TArray<ATargetPoint*>& InPoints);

	UFUNCTION(BlueprintCallable, Category = "Turret")
		ATCannon* GetCannon();

	UFUNCTION(BlueprintPure, Category = "Turret")
		FVector GetTurretForwardVector();

	UFUNCTION(BlueprintPure, Category = "AI|Move params")
		const TArray<ATargetPoint*>& GetPatrollingPoints() const;

	UFUNCTION(BlueprintPure, Category = "AI|Move params")
		float GetMovementAccuracy();

	int32 GetScoresForKilling() const override;

private:
	UPROPERTY()
		ATCannon* TActiveCannon = nullptr;

	UPROPERTY()
		ATCannon* TInactiveCannon = nullptr;

	FVector TurretTargetPosition;
	FVector TurretTargetDirection;

	bool bIsTurretTargetSet = false;

	float TurretRotationAxis = 0.0f;
	float TAxisMoveForward = 0.0f;
	float TAxisRotateRight = 0.0f;
	float TCurrentAxisMoveForward = 0.0f;
	float TCurrentAxisRotateRight = 0.0f;
};
