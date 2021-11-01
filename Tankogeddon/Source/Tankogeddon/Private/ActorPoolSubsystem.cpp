// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorPoolSubsystem.h"


DEFINE_LOG_CATEGORY_CLASS(UActorPoolSubsystem, LogActorPoolSubsystem);


AActor* UActorPoolSubsystem::RetreiveActor(UClass* Class, const FTransform& Transform)
{
	if (!Class)
	{
		return nullptr;
	}

	TArray<AActor*>* Pool = InactiveActors.Find(Class);
	AActor* Result = nullptr;
	if (Pool && Pool->Num() > 0)
	{
		Result = Pool->Pop();
		FVector CorrectedScale = Transform.GetScale3D() * Result->GetActorScale();
		Result->SetActorTransform(FTransform(Transform.GetRotation(), Transform.GetLocation(), CorrectedScale), false, nullptr, ETeleportType::ResetPhysics);
		Result->SetActorTickEnabled(true);
	}
	else
	{
		Result = GetWorld()->SpawnActor(Class, &Transform);
		if (!Result)
		{
			return nullptr;
		}

		Actors.Add(Result);
		Result->OnDestroyed.AddDynamic(this, &UActorPoolSubsystem::OnActorDestroyed);
	}

	return Result;
}

void UActorPoolSubsystem::ReturnActor(AActor* Actor)
{
	if (!Actor || !IsActorInPool(Actor))
	{
		return;
	}

	TArray<AActor*>& Pool = InactiveActors.FindOrAdd(Actor->GetClass());
	if (!Pool.Contains(Actor))
	{
		Actor->SetActorTickEnabled(false);
		Pool.Add(Actor);
	}
}

bool UActorPoolSubsystem::IsActorInPool(AActor* Actor) const
{
	return Actors.Contains(Actor);
}

bool UActorPoolSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UActorPoolSubsystem::OnActorDestroyed(AActor* Actor)
{
	check(IsActorInPool(Actor));
	TArray<AActor*>* Pool = InactiveActors.Find(Actor->GetClass());
	if (Pool && Pool->Contains(Actor))
	{
		Pool->Remove(Actor);
	}

	Actors.Remove(Actor);
}

void UActorPoolSubsystem::DumpPoolStats()
{
	UE_LOG(LogActorPoolSubsystem, Display, TEXT("NumActors in pool: %d"), Actors.Num());
	UE_LOG(LogActorPoolSubsystem, Display, TEXT("InactiveActors in pool:"));
	UE_LOG(LogActorPoolSubsystem, Display, TEXT("Actor\t\t\tClass"));
	UE_LOG(LogActorPoolSubsystem, Display, TEXT("=====\t\t\t====="));
	for (const auto& Record : InactiveActors)
	{
		for (AActor* Actor : Record.Value)
		{
			UE_LOG(LogActorPoolSubsystem, Display, TEXT("%s\t\t\t%s"), *Actor->GetName(), *Record.Key->GetName());
		}
	}
}