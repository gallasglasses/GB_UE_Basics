// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ActorPoolSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TANKOGEDDON_API UActorPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogActorPoolSubsystem, Log, Log);

public:
	AActor* RetreiveActor(UClass* Class, const FTransform& Transform);
	void ReturnActor(AActor* Actor);
	bool IsActorInPool(AActor* Actor) const;

	void DumpPoolStats();

protected:
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

private:
	UFUNCTION()
		void OnActorDestroyed(AActor* Actor);

	UPROPERTY()
		TArray<class AActor*> Actors;

	TMap<UClass*, TArray<class AActor*>> InactiveActors;
};
