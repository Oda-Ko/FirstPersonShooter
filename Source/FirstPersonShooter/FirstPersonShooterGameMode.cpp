// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstPersonShooterGameMode.h"
#include "FirstPersonShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFirstPersonShooterGameMode::AFirstPersonShooterGameMode()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
