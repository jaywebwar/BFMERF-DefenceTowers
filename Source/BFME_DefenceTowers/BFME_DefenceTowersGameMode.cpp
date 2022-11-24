// Copyright Epic Games, Inc. All Rights Reserved.

#include "BFME_DefenceTowersGameMode.h"
#include "BFME_DefenceTowersPlayerController.h"
#include "BFME_DefenceTowersCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABFME_DefenceTowersGameMode::ABFME_DefenceTowersGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ABFME_DefenceTowersPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}