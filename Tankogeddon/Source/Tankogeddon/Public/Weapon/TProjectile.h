// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TProjectile.generated.h"

class UStaticMeshComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class TANKOGEDDON_API ATProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATProjectile();
	virtual void Start();
	virtual void Stop();
	virtual void Explode();
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Fire Parametrs")
		float MoveSpeed = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Fire Parametrs")
		float FireRange = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Fire Parametrs")
		float DamageAmount = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Fire Parametrs")
		float Mass = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion Parametrs", meta = (EditCondition = "bImpactExplosion", EditConditionHides))
		float ExplosionRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion Parametrs", meta = (EditCondition = "bImpactExplosion", EditConditionHides))
		float ExplosionImpulse = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion Parametrs")
		bool bImpactExplosion = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Fire Parametrs")
		UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Fire Parametrs")
		USoundBase* HitAudioEffect;

protected:
	UFUNCTION()
		virtual void OnProjectileHit(UPrimitiveComponent* HittedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult);

private:
	FVector StartPosition;

	void CheckDamageTaken(AActor* DamageTakenActor);
	void CheckSimulatingPhysics(UPrimitiveComponent* POtherComp, const FHitResult& PHitResult);
	void CheckSimulatingPhysics(UPrimitiveComponent* PHitMesh, AActor* PHitActor);
	void SpawnEffects();
};
