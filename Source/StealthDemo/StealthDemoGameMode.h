// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/GameMode.h"
#include "StealthDemoGameMode.generated.h"

enum class EPlayState : short
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(minimalapi)
class AStealthDemoGameMode : public AGameMode
{
	GENERATED_BODY()

	// Members
	EPlayState CurrentState;

	// Overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Helpers
	void HandleNewState(EPlayState NewState);

public:
	AStealthDemoGameMode(const FObjectInitializer& ObjectInitializer);

	// Getters
	EPlayState GetCurrentState() const;

	// Setters
	void SetCurrentState(EPlayState NewState);
};



