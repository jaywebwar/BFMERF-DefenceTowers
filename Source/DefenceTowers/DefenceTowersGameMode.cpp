// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefenceTowersGameMode.h"
#include "DefenceTowersPlayerController.h"
#include "DefenceTowersCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADefenceTowersGameMode::ADefenceTowersGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ADefenceTowersPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}