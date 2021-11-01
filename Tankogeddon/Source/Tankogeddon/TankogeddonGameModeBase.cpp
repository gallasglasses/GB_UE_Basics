// Copyright Epic Games, Inc. All Rights Reserved.
#include "TankogeddonGameModeBase.h"
#include "Player/TPlayerController.h"
#include "Components/Scoreable.h"

DEFINE_LOG_CATEGORY(LogTankogeddon);

void ATankogeddonGameModeBase::DestructionControl(AActor* Actor, const FDamageData& DamageData)
{
	if (IScoreable* Scoreable = Cast<IScoreable>(Actor))
	{
		ATPlayerController* PlayerController = Cast<ATPlayerController>(GetWorld()->GetFirstPlayerController());
		if (DamageData.Instigator == PlayerController->GetPawn())
		{
			PlayerController->Scores += Scoreable->GetScoresForKilling();
			UE_LOG(LogTankogeddon, Log, TEXT("New player scores: %d"), PlayerController->Scores);
		}
	}
}