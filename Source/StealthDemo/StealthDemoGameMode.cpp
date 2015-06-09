// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "StealthDemo.h"
#include "StealthDemoGameMode.h"
#include "GameFramework/HUD.h"

AStealthDemoGameMode::AStealthDemoGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonBP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// set HUD
	static ConstructorHelpers::FClassFinder<AHUD> HUDClassFinder(TEXT("/Game/FirstPersonBP/Blueprints/FirstPersonHUD"));
	HUDClass = HUDClassFinder.Class;

	// set player controller class
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(TEXT("/Game/FirstPersonBP/Blueprints/VRController"));
	PlayerControllerClass = PlayerControllerClassFinder.Class;
}

void AStealthDemoGameMode::Tick(float DeltaSeconds)
{

}

void AStealthDemoGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AStealthDemoGameMode::SetCurrentState(EPlayState NewState)
{
	CurrentState = NewState;

	HandleNewState(NewState);
}

void AStealthDemoGameMode::HandleNewState(EPlayState NewState)
{
	switch (NewState)
	{
	case EPlayState::EPlaying:
		// TODO
		break;
	case EPlayState::EGameOver:
		// TODO
		break;
	default:
		break;
	}
}