// Fill out your copyright notice in the Description page of Project Settings.

#include "StealthDemo.h"
#include "MonsterCharacter.h"
#include "MonsterController.h"

// Sets default values
AMonsterCharacter::AMonsterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Initialize pawn sensing component
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComponent->HearingThreshold = 1000.0f;
	PawnSensingComponent->SensingInterval = 0.1f;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	static_cast<AMonsterController*>(GetController())->Initialize();
}

// Called every frame
void AMonsterCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AMonsterCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

