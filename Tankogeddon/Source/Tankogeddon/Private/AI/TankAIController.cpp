// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TankAIController.h"
#include "Player/TPawn.h"
#include "DrawDebugHelpers.h"
#include "Engine/TargetPoint.h"

void ATankAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATankAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TankPawn = Cast<ATPawn>(GetPawn());
	if (TankPawn)
	{
		MoveToNextPoint();
		Targeting();
	}
}

void ATankAIController::MoveToNextPoint()
{
	const TArray<ATargetPoint*>& PatrollingPoints = TankPawn->GetPatrollingPoints();
	if (PatrollingPoints.Num() == 0)
	{
		return;
	}

	TankPawn->MoveForward(1.f);
	FVector PawnLocation = TankPawn->GetActorLocation();
	FVector CurrentPoint = PatrollingPoints[CurrentPatrolPointIndex]->GetActorLocation();
	if (FVector::DistSquared(PawnLocation, CurrentPoint) <= FMath::Square(TankPawn->GetMovementAccuracy()))
	{
		CurrentPatrolPointIndex++;
		if (!PatrollingPoints.IsValidIndex(CurrentPatrolPointIndex))
		{
			CurrentPatrolPointIndex = 0;
		}
	}

	FVector MoveDirection = CurrentPoint - PawnLocation;
	MoveDirection.Normalize();
	FVector ForwardDirection = TankPawn->GetActorForwardVector();
	FVector RightDirection = TankPawn->GetActorRightVector();

	DrawDebugLine(GetWorld(), PawnLocation, CurrentPoint, FColor::Green, false, 0.1f, 0, 5);

	float ForwardAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardDirection, MoveDirection)));
	float RightAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RightDirection, MoveDirection)));

	float RotationValue = 0.0f;
	if (ForwardAngle > 5.0f)
		RotationValue = 1;
	if (RightAngle > 90.0f)
		RotationValue = -RotationValue;

	TankPawn->RotateRight(RotationValue);
}

void ATankAIController::Targeting()
{
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!PlayerPawn)
	{
		return;
	}
	if (FVector::DistSquared(PlayerPawn->GetActorLocation(), TankPawn->GetActorLocation()) > FMath::Square(TargetingRange))
	{
		return;
	}

	FHitResult HitResult;
	FVector TraceStart = TankPawn->GetActorLocation();
	FVector TraceEnd = PlayerPawn->GetActorLocation();
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("AI Vission Trace")), true, TankPawn);
	TraceParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
	{
		DrawDebugLine(GetWorld(), TraceStart, HitResult.Location, FColor::Red, false, 0.1f, 0, 5);
		if (HitResult.Actor != PlayerPawn)
		{
			return;
		}
	}
	else
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 0.1f, 0, 5);
	}

	TankPawn->SetTurretTargetPosition(PlayerPawn->GetActorLocation());

	FVector TargetingDir = TankPawn->GetTurretForwardVector();
	FVector DirToPlayer = PlayerPawn->GetActorLocation() - TankPawn->GetActorLocation();
	DirToPlayer.Normalize();
	float AimAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(TargetingDir, DirToPlayer)));

	if (AimAngle <= Accuracy)
	{
		TankPawn->Fire();
	}
}
