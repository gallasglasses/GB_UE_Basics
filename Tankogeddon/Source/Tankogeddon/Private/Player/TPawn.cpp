
// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/TPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
}

// Called when the game starts or when spawned
void ATPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // forward or backward movement
    FVector NewLocation = GetActorLocation() + GetActorForwardVector() * TAxisMoveForward * MoveSpeed * DeltaTime;
    SetActorLocation(NewLocation);

    // movement to the right or left
    /*NewLocation = GetActorLocation() + GetActorRightVector() * MoveSpeed * TAxisMoveRight * DeltaTime;
    SetActorLocation(NewLocation);*/

    //rotation movement
    float NewYawRotation = GetActorRotation().Yaw + TAxisMoveRight * RotationSpeed * DeltaTime;
    SetActorRotation(FRotator(0, NewYawRotation, 0));
}

void ATPawn::MoveForward(float Amount)
{
    TAxisMoveForward = Amount;
}

void ATPawn::MoveRight(float Amount)
{
    TAxisMoveRight = Amount;
}