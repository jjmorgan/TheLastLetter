// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "AIController.h"
#include "MonsterController.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHDEMO_API AMonsterController : public AAIController
{
	GENERATED_BODY()
	
public:
	// Constructor
	AMonsterController(const FObjectInitializer& ObjectInitializer);

	// Possess character
	virtual void Possess(class APawn *Inpawn) override;

	// Initialize blackboard and behavior tree
	void Initialize();

	// Run behavior tree
	UFUNCTION(BlueprintImplementableEvent, Category = Gameplay)
		void Begin();

	// Get closest patrol point
	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void ClosestPatrolPoint(TArray<class APatrolPoint*>& PatrolPointResult);

	// Get next patrol point
	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void NextPatrolPoint(TArray<class APatrolPoint*>& PatrolPointResult);

	// Reached patrol point
	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void ReachedPatrolPoint();

protected:
	// Blackboard asset
	UBlackboardComponent *Blackboard;

	// Patrol points to follow
	TArray<class APatrolPoint*> PatrolPoints;

	// Current patrol point index
	int32 PatrolIndex;

public:
};
