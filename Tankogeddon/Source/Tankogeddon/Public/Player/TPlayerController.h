
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

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY()
		ATPawn* TPawn;

	void MoveForward(float Amount);
	void RotateRight(float Amount);
	void Fire();
};
