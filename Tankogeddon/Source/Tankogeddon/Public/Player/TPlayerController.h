
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPlayerController.generated.h"

class ATPawn;

UCLASS()
class TANKOGEDDON_API ATPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
		int32 Scores;

private:
	UPROPERTY()
		ATPawn* TPawn;

	UFUNCTION(exec)
		void DumpActorPoolSubsystemStats();

	void MoveForward(float Amount);
	void RotateRight(float Amount);
	void Fire();
	void StartRifleFire();
	void NextWeapon();
};
