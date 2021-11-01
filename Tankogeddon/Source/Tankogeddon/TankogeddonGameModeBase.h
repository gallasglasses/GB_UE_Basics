// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameStructs.h"
#include "GameFramework/GameModeBase.h"
#include "TankogeddonGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TANKOGEDDON_API ATankogeddonGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void DestructionControl(AActor* Actor, const FDamageData& DamageData);
};
