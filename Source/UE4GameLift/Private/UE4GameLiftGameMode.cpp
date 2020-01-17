// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UE4GameLiftGameMode.h"
#include "UE4GameLiftCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE4GameLiftGameMode::AUE4GameLiftGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
