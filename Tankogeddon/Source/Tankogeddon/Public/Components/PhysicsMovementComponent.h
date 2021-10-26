// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKOGEDDON_API UPhysicsMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPhysicsMovementComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity params")
		FVector Gravity = FVector(0.f, 0.f, -9.8f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity params")
		FVector Velocity = FVector::ZeroVector;

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
