// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeRewindGameMode.h"
#include "TimeRewindCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATimeRewindGameMode::ATimeRewindGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
