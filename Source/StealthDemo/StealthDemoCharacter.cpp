// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "StealthDemo.h"
#include "StealthDemoCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/InputSettings.h"
#include "Engine.h"

#include <sstream>

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AStealthDemoCharacter

AStealthDemoCharacter::AStealthDemoCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/// Mesh is inherited

	/*
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetMesh();
	//FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	*/

	CurrentItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CurrentItemMesh"));
	CurrentItemMesh->AttachTo(GetMesh(), FName("RightHand"));
	CurrentItemMesh->SetRelativeLocation(FVector(0.0f, -7.0f, -1.0f));
	CurrentItemMesh->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	StaminaAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StaminaAudioComponent"));

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// initialize stamina
	Stamina = 150.0f;
	StaminaState = SS_Normal;

	// other attributes
	IsRunning = false;
	HoldingItem = false;
	LockMovement = false;
	PickedUpNew = false;
	Hiding = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AStealthDemoCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Run", IE_Pressed, this, &AStealthDemoCharacter::BeginRunning);
	InputComponent->BindAction("Run", IE_Released, this, &AStealthDemoCharacter::StopRunning);

	InputComponent->BindAction("Use", IE_Pressed, this, &AStealthDemoCharacter::UseItem);
	InputComponent->BindAction("Use", IE_Released, this, &AStealthDemoCharacter::UseItemRelease);

	InputComponent->BindAction("NextItem", IE_Pressed, this, &AStealthDemoCharacter::NextItem);

	InputComponent->BindAction("QuickTurn", IE_Pressed, this, &AStealthDemoCharacter::QuickTurn);

	InputComponent->BindAxis("MoveForward", this, &AStealthDemoCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AStealthDemoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an bagsolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AStealthDemoCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AStealthDemoCharacter::LookUpAtRate);
}

void AStealthDemoCharacter::MoveForward(float Value)
{
	if (Value != 0.0f && !LockMovement && !Hiding)
	{
		if (!IsRunning)
			Value *= 0.4;

		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AStealthDemoCharacter::MoveRight(float Value)
{
	if (Value != 0.0f && !LockMovement && !Hiding)
	{
		if (!IsRunning)
			Value *= 0.4f;

		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AStealthDemoCharacter::BeginRunning()
{
	if (StaminaState != SS_Depleted)
		IsRunning = true;
}

void AStealthDemoCharacter::StopRunning()
{
	IsRunning = false;
}

void AStealthDemoCharacter::QuickTurn()
{
	MoveToPosition(GetActorLocation(), GetControlRotation().Yaw - 180.0f, 2.5f);
}

void AStealthDemoCharacter::MoveToPosition(FVector to_position, float to_yaw, float speed)
{
	if (LockMovement)
		return;

	FVector from_loc = GetActorLocation();
	FRotator from_rot = GetControlRotation();

	// from
	MTPPositionA = from_loc;
	MTPRotatorA = from_rot;

	// to
	MTPPositionB = FVector(to_position.X, to_position.Y, from_loc.Z);
	MTPRotatorB = FRotator(from_rot.Pitch, to_yaw, from_rot.Roll);

	MTPSpeed = speed;

	// Lerp and timeline are controlled in event blueprint
	OnMoveToPositionDelegate.Broadcast();
}

void AStealthDemoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AStealthDemoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (Rate > 0.0) {
		FString RateFloat = FString::SanitizeFloat(Rate);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, RateFloat);
	}
		
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//////////////////////////////////////////////////////////////////////////
// Interaction

void AStealthDemoCharacter::UseItem()
{
	if (InventoryIndex > 0 && Inventory[InventoryIndex - 1].throwable) {
		BeginAim();
	}
	else
		OnUseItemDelegate.Broadcast();
}

void AStealthDemoCharacter::UseItemRelease()
{
	if (!PickedUpNew && InventoryIndex > 0 && Inventory[InventoryIndex - 1].throwable) {
		bool can_throw;
		IsThrowPossible(can_throw);

		if (can_throw) {
			ThrowItem(Inventory[InventoryIndex - 1].item_mesh, Inventory[InventoryIndex - 1].pickup_actor);
			RemoveCurrentInventoryItem();
		}
			
		EndAim();
	}

	PickedUpNew = false;
}

void AStealthDemoCharacter::NextItem()
{
	InventoryIndex++;
	SwitchHoldingItem();
}

void AStealthDemoCharacter::AddInventoryItem(int32 id, UStaticMesh *item_mesh, AActor *pickup_actor, FVector hand_position, FRotator hand_rotation, bool throwable)
{
	InventoryItem newitem;
	newitem.id = id;
	newitem.item_mesh = item_mesh;
	newitem.pickup_actor = pickup_actor;
	newitem.position = hand_position;
	newitem.rotation = hand_rotation;
	newitem.throwable = throwable;

	Inventory.push_back(newitem);
	
	PickedUpNew = true;

	InventoryIndex = Inventory.size();
	SwitchHoldingItem();
}

void  AStealthDemoCharacter::CheckHoldingItem(int32 id, UStaticMesh *item_mesh, bool& result)
{
	if (InventoryIndex == 0) {
		result = false;
		return;
	}

	InventoryItem current = Inventory[InventoryIndex - 1];
	result = (current.id == id && current.item_mesh == item_mesh);
}

void AStealthDemoCharacter::SwitchHoldingItem()
{
	if (InventoryIndex > Inventory.size())
		InventoryIndex = 0;

	if (CurrentItemMesh != NULL) {
		CurrentItemMesh->SetStaticMesh(NULL);
	}

	HoldingItem = false;

	// Delay animation between switching items
	if (InventoryIndex > 0)
		GetWorldTimerManager().SetTimer(SwitchItemTimer, this, &AStealthDemoCharacter::SwitchHoldingItemFinal, 0.15f, false);
}

void AStealthDemoCharacter::SwitchHoldingItemFinal()
{
	InventoryItem item = Inventory[InventoryIndex - 1];

	if (CurrentItemMesh != NULL) {
		CurrentItemMesh->SetStaticMesh(item.item_mesh);
		CurrentItemMesh->SetRelativeLocation(item.position);
		CurrentItemMesh->SetRelativeRotation(item.rotation);
	}

	HoldingItem = true;

	GetWorldTimerManager().ClearTimer(SwitchItemTimer);
}

void AStealthDemoCharacter::RemoveCurrentInventoryItem()
{
	if (InventoryIndex > 0)
	{
		Inventory.erase(Inventory.begin() + (InventoryIndex - 1));
		InventoryIndex = 0;
		SwitchHoldingItem();
	}
}

//////////////////////////////////////////////////////////////////////////
// Tick

void AStealthDemoCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Stamina
	if (IsRunning) {
		if (Stamina > 0.0f)
			Stamina -= 0.25f;
	}
	else if (Stamina < 100.0f) {
		if (GetVelocity().Size() > 0.0f)
			Stamina += 0.1;
		else
			Stamina += 0.2;
	}
	if (Stamina < 0.0f)
		Stamina = 0.0f;
	else if (Stamina > 150.0f)
		Stamina = 150.0f;

	// State machine for controlling stamina sounds
	switch (StaminaState) {
	case SS_Normal:
		if (Stamina < 80.0f) {
			// Play running sound
			StaminaAudioComponent->SetSound(StaminaRunningSound);
			StaminaAudioComponent->Play();
			StaminaState = SS_Running;
		}
		break;
	case SS_Running:
		if (Stamina == 0.0f) {
			// Play depleted sound
			StaminaAudioComponent->Stop();
			StaminaAudioComponent->SetSound(StaminaDepletedSound);
			StaminaAudioComponent->Play();
			StaminaState = SS_Depleted;
			// Running disabled temporarily
			StopRunning();
		}
		else if (!IsRunning) {
			// Play recover sound
			StaminaAudioComponent->Stop();
			StaminaAudioComponent->SetSound(StaminaRecoveringSound);
			StaminaAudioComponent->Play();
			StaminaState = SS_Recovering;
		}
		break;
	case SS_Depleted:
		if (Stamina > 50.0f) {
			// Play recover sound
			StaminaAudioComponent->Stop();
			StaminaAudioComponent->SetSound(StaminaRecoveringSound);
			StaminaAudioComponent->Play();
			StaminaState = SS_Recovering;
		}
		break;
	case SS_Recovering:
		if (IsRunning) {
			// Play running sound
			StaminaAudioComponent->Stop();
			StaminaAudioComponent->SetSound(StaminaRunningSound);
			StaminaAudioComponent->Play();
			StaminaState = SS_Running;
		}
		else if (Stamina > 80.0f) {
			// Stop sound
			StaminaAudioComponent->Stop();
			StaminaState = SS_Normal;
		}
		break;
	}
}