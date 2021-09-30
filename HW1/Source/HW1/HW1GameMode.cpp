// Copyright Epic Games, Inc. All Rights Reserved.

#include "HW1GameMode.h"
#include "HW1Character.h"
#include "UObject/ConstructorHelpers.h"

AHW1GameMode::AHW1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
