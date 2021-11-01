// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/TAmmoPickup.h"
#include "Components/StaticMeshComponent.h"
#include "TPawn.h"
#include "Weapon/TCannon.h"

ATAmmoPickup::ATAmmoPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ATAmmoPickup::OnMeshOverlapBegin);
	Mesh->SetCollisionProfileName(FName("OverlapOnlyPawn"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetGenerateOverlapEvents(true);
}

void ATAmmoPickup::OnMeshOverlapBegin(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATPawn* PlayerPawn = Cast<ATPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	if (OtherActor == PlayerPawn)
	{
		ATCannon* Cannon = PlayerPawn->GetCannon();
		if (Cannon && Cannon->GetClass() == CannonClass)
		{
			Cannon->AddAmmo(Cannon->GetCannonType(), Bullets);
		}
		else
		{
			PlayerPawn->SetupCannon(CannonClass);
			Cannon = PlayerPawn->GetCannon();
			Cannon->AddAmmo(Cannon->GetCannonType(), Bullets);
		}
		Destroy();
	}
}