// Fill out your copyright notice in the Description page of Project Settings.

#include "StealthDemo.h"
#include "MonsterController.h"
#include "PatrolPoint.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"

AMonsterController::AMonsterController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AMonsterController::Initialize()
{
	// Blackboard and behavior tree are set in blueprint
	Blackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::FromInt(Blackboard->GetNumKeys()));

	// Initialize blackboard values
	//Blackboard->SetValueAsBool(Blackboard->GetKeyID("InRoom"), false);
	Blackboard->SetValueAsBool(Blackboard->GetKeyID("NearDoor"), false);
	Blackboard->SetValueAsBool(Blackboard->GetKeyID("BeginPatrol"), true);
	Blackboard->SetValueAsVector(Blackboard->GetKeyID("NextLoc"), FVector(0.0f, 0.0f, 0.0f));

	// Get and sort patrol points in the level
	for (TActorIterator<APatrolPoint> PatrolPointItr(GetWorld()); PatrolPointItr; ++PatrolPointItr)
	{
		PatrolPoints.Add(*PatrolPointItr);
	}
	PatrolPoints.Sort([](const APatrolPoint& One, const APatrolPoint& Two) {
		if (One.Tags.Num() == 0 || Two.Tags.Num() == 0)
			return false;
		return FCString::Atoi(*One.Tags[0].ToString()) < FCString::Atoi(*Two.Tags[0].ToString());
	});
	PatrolIndex = 0;

	// Start behavior (blueprint)
	Begin();
}

void AMonsterController::Possess(APawn *InPawn)
{
	Super::Possess(InPawn);
}

void AMonsterController::ClosestPatrolPoint(TArray<APatrolPoint*>& PatrolPointResult)
{
	APatrolPoint *closest = NULL;
	float MinDistance = 0.0f;
	bool first = true;

	for (int32 i = 0; i < PatrolPoints.Num(); i++) {
		float Distance = GetControlledPawn()->GetDistanceTo(PatrolPoints[i]);
		if (first || Distance < MinDistance) {
			MinDistance = Distance;
			closest = PatrolPoints[i];
			PatrolIndex = i;
			first = false;
		}
	}

	if (closest)
		PatrolPointResult.Add(closest);
}

void AMonsterController::NextPatrolPoint(TArray<APatrolPoint*>& PatrolPointResult)
{
	PatrolPointResult.Add(PatrolPoints[PatrolIndex]);
}

void AMonsterController::ReachedPatrolPoint()
{
	PatrolIndex++;
	if (PatrolIndex >= PatrolPoints.Num())
		PatrolIndex = 0;
}
