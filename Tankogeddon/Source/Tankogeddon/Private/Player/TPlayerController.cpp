
// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/TPlayerController.h"
#include "Player/TPawn.h"
#include "DrawDebugHelpers.h"
#include "ActorPoolSubsystem.h"

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
	InputComponent->BindAction("RifleFire", IE_Pressed, this, &ATPlayerController::StartRifleFire);
    InputComponent->BindAction("NextWeapon", IE_Pressed, this, &ATPlayerController::NextWeapon);
    InputComponent->BindAxis("RotateTurretRight");
}

void ATPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (!TPawn)
	{
		return;
	}

	FVector2D MouseScreenPosition;
	GetMousePosition(MouseScreenPosition.X, MouseScreenPosition.Y);
	bool bWasMouseMoved = !LastFrameMousePosition.Equals(MouseScreenPosition);
	LastFrameMousePosition = MouseScreenPosition;

	float TurretRotationAxis = GetInputAxisValue("RotateTurretRight");
	if (FMath::IsNearlyZero(TurretRotationAxis) && (bWasMouseMoved || bIsControllingFromMouse))
	{
		bIsControllingFromMouse = true;
		FVector WorldMousePosition, MouseDirection;
		DeprojectMousePositionToWorld(WorldMousePosition, MouseDirection);

		FVector PawnPos = TPawn->GetActorLocation();
		WorldMousePosition.Z = PawnPos.Z;
		FVector NewTurretDirection = WorldMousePosition - PawnPos;
		NewTurretDirection.Normalize();

		FVector TurretTarget = PawnPos + NewTurretDirection * 1000.f;
		TPawn->SetTurretTargetPosition(TurretTarget);
	}
	else
	{
		bIsControllingFromMouse = false;
		TPawn->SetTurretRotationAxis(TurretRotationAxis);
	}

	DrawDebugLine(GetWorld(), TPawn->GetActorLocation(), TPawn->GetActorLocation() + TPawn->GetTurretForwardVector() * 1000.f, FColor::Green, false, 0.1f, 0.f, 5.f);
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

void ATPlayerController::StartRifleFire()
{
	if (TPawn)
	{
		TPawn->StartRifleFire();
	}
}

void ATPlayerController::NextWeapon()
{
    if (TPawn)
    {
        TPawn->NextWeapon();
    }
}

void ATPlayerController::DumpActorPoolSubsystemStats()
{
	GetWorld()->GetSubsystem<UActorPoolSubsystem>()->DumpPoolStats();
}