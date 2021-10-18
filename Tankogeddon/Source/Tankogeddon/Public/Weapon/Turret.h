// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Damageable.h"
#include "Components/Scoreable.h"
#include "Turret.generated.h"

class UStaticMeshComponent;
class UStaticMeshComponent;
class UArrowComponent;
class UBoxComponent;
class UHealthComponent;
class ATCannon;
class APawn;

UCLASS()
class TANKOGEDDON_API ATurret : public AActor, public IDamageable, public IScoreable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurret();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* TurretMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UArrowComponent* CannonSetupPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UBoxComponent* HitCollider;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		TSubclassOf<ATCannon> CannonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float TargetingRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float TargetingSpeed = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float Accuracy = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float ScoresForKilling = 10.f;

	const FString BodyMeshPath = "StaticMesh'/Game/CSC/Meshes/SM_CSC_Tower1.SM_CSC_Tower1'";
	const FString TurretMeshPath = "StaticMesh'/Game/CSC/Meshes/SM_CSC_Gun1.SM_CSC_Gun1'";

	// Called when the game starts or when spawned
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void TakeDamage(const FDamageData& DamageData) override;
	virtual int32 GetScoresForKilling() const override;
};
