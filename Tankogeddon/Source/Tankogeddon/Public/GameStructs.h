#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "GameStructs.generated.h"

UENUM(BlueprintType)
enum class ECannonType : uint8
{
	FireProjectile = 0 UMETA(Display = "Use Projectile"),
	FireTrace = 1 UMETA(Display = "Use Trace"),
	FireRifle = 2 UMETA(Display = "Use Rifle")
};