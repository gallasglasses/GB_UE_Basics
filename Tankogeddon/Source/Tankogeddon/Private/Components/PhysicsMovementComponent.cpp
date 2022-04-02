// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PhysicsMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UPhysicsMovementComponent::UPhysicsMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
}


// Called when the game starts
void UPhysicsMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPhysicsMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector NewActorLocation = Owner->GetActorLocation() + Velocity * DeltaTime - FVector::UpVector * Gravity * FMath::Square(DeltaTime) / 2.f;
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, FString::Printf(TEXT("TargetRotation.Pitch %f"), Owner->GetActorRotation().Pitch));
	Velocity += FVector::UpVector * (-Gravity) * DeltaTime;
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Blue, FString::Printf(TEXT("Velocity %f"), Velocity.Z));
	Owner->SetActorLocation(NewActorLocation, true);
}
