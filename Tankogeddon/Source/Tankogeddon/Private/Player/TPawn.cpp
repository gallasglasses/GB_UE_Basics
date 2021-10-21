
// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/TPawn.h"
#include "..\..\Public\Weapon\TCannon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameStructs.h"
#include "HealthComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/TAmmoPickup.h"
#include "Particles/ParticleSystem.h"

//DEFINE_LOG_CATEGORY(LogPawn);

// Sets default values
ATPawn::ATPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    S_TBody = CreateDefaultSubobject<UStaticMeshComponent>("TankBody");
    RootComponent = S_TBody;

    S_TTurret = CreateDefaultSubobject<UStaticMeshComponent>("TankTurret");
    S_TTurret->SetupAttachment(S_TBody);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
    SpringArm->SetupAttachment(S_TBody);
    SpringArm->bDoCollisionTest = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;
    SpringArm->TargetArmLength = 2500.0f;
    SpringArm->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
    SpringArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

    Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
    Camera->SetupAttachment(SpringArm);

	CannonSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon spawn point"));
    CannonSpawnPoint->SetupAttachment(S_TTurret);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(S_TBody);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ATPawn::OnHealthChanged);
	HealthComponent->OnDie.AddDynamic(this, &ATPawn::OnDie);
}

// Called when the game starts or when spawned
void ATPawn::BeginPlay()
{
	Super::BeginPlay();

    SetupCannon(DefaultCannonClass);
}

// Called every frame
void ATPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // forward or backward movement
    //TCurrentAxisMoveForward = FMath::Lerp(TCurrentAxisMoveForward, TAxisMoveForward, MovementSmoothness);
    FVector NewLocation = GetActorLocation() + GetActorForwardVector() * TCurrentAxisMoveForward * MoveSpeed * DeltaTime;
    SetActorLocation(NewLocation, true);

    //rotation movement
    //TCurrentAxisRotateRight = FMath::Lerp(TCurrentAxisRotateRight, TAxisRotateRight, RotationSmoothness);
    float NewYawRotation = GetActorRotation().Yaw + TCurrentAxisRotateRight * RotationSpeed * DeltaTime;
    SetActorRotation(FRotator(0.f, NewYawRotation, 0.f));
   
    //UE_LOG(LogPawn, Verbose, TEXT("TCurrentAxisRotateRight: %f"), TCurrentAxisRotateRight);

	if (!bIsTurretTargetSet)
	{
		TurretTargetDirection = TurretTargetDirection.RotateAngleAxis(RotationSmoothness * TurretRotationAxis, FVector::UpVector);
		TurretTargetPosition = GetActorLocation() + TurretTargetDirection;
	}

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(S_TTurret->GetComponentLocation(), TurretTargetPosition);
	FRotator CurrentRotation = S_TTurret->GetComponentRotation();
	TargetRotation.Roll = CurrentRotation.Roll;
	TargetRotation.Pitch = CurrentRotation.Pitch;
    S_TTurret->SetWorldRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed), true);
}

void ATPawn::MoveForward(float Amount)
{
    TAxisMoveForward = Amount;
    TCurrentAxisMoveForward = FMath::Lerp(TCurrentAxisMoveForward, TAxisMoveForward, MovementSmoothness * GetWorld()->DeltaTimeSeconds);
}

void ATPawn::RotateRight(float Amount)
{
    TAxisRotateRight = Amount;
    TCurrentAxisRotateRight = FMath::Lerp(TCurrentAxisRotateRight, TAxisRotateRight, RotationSmoothness * GetWorld()->DeltaTimeSeconds);
}

void ATPawn::SetTurretTargetPosition(const FVector& TargetPosition)
{
	TurretTargetPosition = TargetPosition; 
	bIsTurretTargetSet = true;
}

void ATPawn::SetTurretRotationAxis(float AxisValue)
{
	TurretRotationAxis = AxisValue;
	TurretTargetDirection = TurretTargetPosition - GetActorLocation();
	bIsTurretTargetSet = false;
}

void ATPawn::Fire()
{
	if (TActiveCannon)
	{
		TActiveCannon->Fire();
	}
}

void ATPawn::StartRifleFire()
{
    if (TActiveCannon)
    {
        TActiveCannon->StartRifleFire();
    }
}

void ATPawn::SetupCannon(TSubclassOf<ATCannon> InCannonClass)
{
	if (TActiveCannon && TActiveCannon->GetClass() != InCannonClass)
	{
        TInactiveCannon = TActiveCannon;
	}
	if (InCannonClass)
	{
		FActorSpawnParameters Params;
		Params.Instigator = this;
		Params.Owner = this;
		TActiveCannon = GetWorld()->SpawnActor<ATCannon>(InCannonClass, Params);
		TActiveCannon->AttachToComponent(CannonSpawnPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void ATPawn::NextWeapon()
{
    if (TActiveCannon && TInactiveCannon)
    {
		Swap(TActiveCannon, TInactiveCannon);

		if (TActiveCannon)
		{
			TActiveCannon->SetVisibility(true);
		}

		if (TInactiveCannon)
		{
			TInactiveCannon->SetVisibility(false);
		}
    }
}

void ATPawn::SetPatrollingPoints(const TArray<ATargetPoint*>& InPoints)
{
	PatrollingPoints = InPoints;
}

ATCannon* ATPawn::GetCannon()
{
    return TActiveCannon;
}

FVector ATPawn::GetTurretForwardVector()
{
    return S_TTurret->GetForwardVector();
}

const TArray<ATargetPoint*>& ATPawn::GetPatrollingPoints() const
{
    return PatrollingPoints;
}

void ATPawn::OnHealthChanged_Implementation(float Damage)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Purple, FString::Printf(TEXT("Turret %s taked damage:%f "), *GetName(), Damage));
}

void ATPawn::OnDie_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathEffect, GetActorTransform().GetLocation(), GetActorTransform().GetRotation().Rotator(), FVector(3.0, 3.0, 3.0), true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathAudioEffect, GetActorLocation());

	if (LootBox)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoFail = true;
		GetWorld()->SpawnActor<ATAmmoPickup>(LootBox, GetActorTransform(), SpawnParams);
	}
	Destroy();
}

void ATPawn::TakeDamage(const FDamageData& DamageData)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, GetActorTransform().GetLocation(), GetActorTransform().GetRotation().Rotator(), FVector(3.0, 3.0, 3.0), true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitAudioEffect, GetActorLocation());
	HealthComponent->TakeDamage(DamageData);
}

float ATPawn::GetMovementAccuracy()
{
    return MovementAccuracy;
}

int32 ATPawn::GetScoresForKilling() const
{
	return ScoresForKilling;
}