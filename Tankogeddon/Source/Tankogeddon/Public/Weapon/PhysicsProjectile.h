// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/TProjectile.h"
#include "PhysicsProjectile.generated.h"

class UPhysicsMovementComponent;

UCLASS()
class TANKOGEDDON_API APhysicsProjectile : public ATProjectile
{
	GENERATED_BODY()
public:
	APhysicsProjectile();

	virtual void Start() override;
	virtual void Stop() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UPhysicsMovementComponent* MovementComponent;
};
