
// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/TPlayerController.h"
#include "Player/TPawn.h"

void ATPlayerController::BeginPlay()
{
    Super::BeginPlay();

    TPawn = Cast<ATPawn>(GetPawn());
}

// Called to bind functionality to input
void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &ATPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATPlayerController::MoveRight);
}

void ATPlayerController::MoveForward(float Amount)
{
    if (TPawn)
    {
        TPawn->MoveForward(Amount);
    }
}

void ATPlayerController::MoveRight(float Amount)
{
    if (TPawn)
    {
        TPawn->MoveRight(Amount);
    }
}