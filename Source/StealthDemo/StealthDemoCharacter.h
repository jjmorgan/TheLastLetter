// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include <vector>
#include "GameFramework/Character.h"
#include "StealthDemoCharacter.generated.h"

class UInputComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveToPositionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUseItemDelegate);

UCLASS(config = Game)
class AStealthDemoCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	/** COMPONENTS **/

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	class UStaticMeshComponent *CurrentItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Audio)
	class UAudioComponent *StaminaAudioComponent;

	/** RESOURCES **/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundWave *StaminaRunningSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundWave *StaminaDepletedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundWave *StaminaRecoveringSound;

public:

	/** ATTRIBUTES **/

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Stamina of character */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
		float Stamina;

	/** Indicates if the player is running */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		bool IsRunning;

	/** Holding item animation toggle */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
		bool HoldingItem;

	/** Lock movement, e.g. for move to position */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		bool LockMovement;

	/** Hiding (prevents movement) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool Hiding;

	/** In shadow */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool InShadow;

	/** DELEGATES **/

	UPROPERTY(BlueprintAssignable, Category = Movement)
		FMoveToPositionDelegate OnMoveToPositionDelegate;

	UPROPERTY(BlueprintAssignable, Category = Gameplay)
		FUseItemDelegate OnUseItemDelegate;

protected:

	/** OVERRIDES **/

	virtual void Tick(float DeltaSeconds) override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/** MOVEMENT **/

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/** Run movement state */
	void BeginRunning();

	/** Walk movement state */
	void StopRunning();

	/** Quick 180-degree turn */
	void QuickTurn();

	/** Variables used for move to position **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		FVector MTPPositionA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		FVector MTPPositionB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		FRotator MTPRotatorA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		FRotator MTPRotatorB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		float MTPSpeed;

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** INTERACTION **/

	struct InventoryItem {
		int32 id;
		UStaticMesh *item_mesh;
		AActor *pickup_actor;
		FVector position;
		FRotator rotation;
		bool throwable;
	};

	/** Use object or item **/
	void UseItem();

	/** Release use key **/
	void UseItemRelease();

	bool PickedUpNew;

	/** Toggle next item **/
	void NextItem();

	/** Switch current item **/
	void SwitchHoldingItem();

	void SwitchHoldingItemFinal();

	FTimerHandle SwitchItemTimer;

	/** Inventory **/
	std::vector<InventoryItem> Inventory;

	int32 InventoryIndex;

	/** STATES **/

	/** Stamina state for sound control */
	enum StaminaStateEnum {
		SS_Normal,
		SS_Running,
		SS_Depleted,
		SS_Recovering
	} StaminaState;

public:
	AStealthDemoCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void MoveToPosition(FVector to_position, float to_yaw, float speed);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void AddInventoryItem(int32 id, UStaticMesh *item_mesh, AActor *pickup_actor, FVector hand_position, FRotator hand_rotation, bool throwable);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void CheckHoldingItem(int32 id, UStaticMesh *item_mesh, bool& result);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void RemoveCurrentInventoryItem();

	UFUNCTION(BlueprintImplementableEvent, Category = Gameplay)
		void BeginAim();

	UFUNCTION(BlueprintImplementableEvent, Category = Gameplay)
		void IsThrowPossible(bool& Result);

	UFUNCTION(BlueprintImplementableEvent, Category = Gameplay)
		void ThrowItem(UStaticMesh* item_mesh, AActor* pickup_actor);

	UFUNCTION(BlueprintImplementableEvent, Category = Gameplay)
		void EndAim();
};
