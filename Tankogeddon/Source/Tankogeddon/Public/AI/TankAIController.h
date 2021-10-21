// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TankAIController.generated.h"

class ATPawn;
/**
 * 
 */
UCLASS()
class TANKOGEDDON_API ATankAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
		ATPawn* TankPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float TargetingRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Targeting")
		float Accuracy = 10.f;

	int32 CurrentPatrolPointIndex = 0;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void MoveToNextPoint();
	void Targeting();
};
