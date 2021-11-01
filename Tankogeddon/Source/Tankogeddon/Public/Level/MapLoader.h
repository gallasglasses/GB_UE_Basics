// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapLoader.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UPointLightComponent;
class UPointLightComponent;
class USoundBase;

UCLASS()
class TANKOGEDDON_API AMapLoader : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapLoader();

	UFUNCTION(BlueprintCallable, Category = "MapLoader")
		void SetIsActivated(bool NewIsActivated);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* GatesMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USphereComponent* TriggerCollider;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UPointLightComponent* ActivatedLight;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UPointLightComponent* DeactivatedLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		USoundBase* ActivatedAudioEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level loading params")
		FName LoadLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level loading params")
		bool bIsActivated = false;
private:
	void SetActiveLights();

	UFUNCTION()
		void OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
