// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "MonsterCharacter.generated.h"

UCLASS()
class STEALTHDEMO_API AMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/** COMPONENTS **/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UPawnSensingComponent *PawnSensingComponent;

public:
	// Sets default values for this character's properties
	AMonsterCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	
	
};
