
// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/TPlayerController.h"
#include "Player/TPawn.h"
#include "DrawDebugHelpers.h"

void ATPlayerController::BeginPlay()
{
    Super::BeginPlay();

    TPawn = Cast<ATPawn>(GetPawn());
    bShowMouseCursor = true;
}

// Called to bind functionality to input
void ATPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &ATPlayerController::MoveForward);
	InputComponent->BindAxis("RotateRight", this, &ATPlayerController::RotateRight);
    InputComponent->BindAction("Fire", IE_Pressed, this, &ATPlayerController::Fire);
}

void ATPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (!TPawn)
	{
		return;
	}

    FVector WorldMousePosition, WorldMouseDirection;
    DeprojectMousePositionToWorld(WorldMousePosition, WorldMouseDirection);

    FVector TurretTargetDirection = WorldMousePosition - TPawn->GetActorLocation();
    TurretTargetDirection.Z = 0.0f;
    TurretTargetDirection.Normalize();
    
    FVector TurretTargetPosition = TPawn->GetActorLocation() + TurretTargetDirection * 1000.0f;

    DrawDebugLine(GetWorld(), TPawn->GetActorLocation(), TurretTargetPosition, FColor::Green, false, 0.1f, 0, 5.0f);

    TPawn->SetTurretTargetPosition(TurretTargetPosition);
}

void ATPlayerController::MoveForward(float Amount)
{
    if (TPawn)
    {
        TPawn->MoveForward(Amount);
    }
}

void ATPlayerController::RotateRight(float Amount)
{
    if (TPawn)
    {
        TPawn->RotateRight(Amount);
    }
}

void ATPlayerController::Fire()
{
	if (TPawn)
	{
		TPawn->Fire();
	}
}
