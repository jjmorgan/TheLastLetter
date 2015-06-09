// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TargetPoint.h"
#include "PatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHDEMO_API APatrolPoint : public ATargetPoint
{
	GENERATED_BODY()

	/* Patrol points are simple markers placed in a level

	   When the MonsterController is initialized it gets all
	   PatrolPoints and sorts them by tag

	   Order is specified using actor tags in the editor
	*/
	
	/*FORCEINLINE bool operator<(const APatrolPoint& A, const APatrolPoint& B)
	{
		if (Tags.Num() == 0 || rhs.Tags.Num() == 0)
			return 0;
		return Tags[0] < rhs.Tags[0];
	}*/
};
