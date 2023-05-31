// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TimeRewindCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ATimeRewindCharacter : public ACharacter
{
	GENERATED_BODY()

	/** First person camera, exposed to blueprint for use */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Side camera, exposed to blueprint for use */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* SideCameraComponent;

	/** Top camera, exposed to blueprint for use */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TopCameraComponent;

	//Array of cameras to swap between
	TArray<UCameraComponent*> CameraComponentList;

	//is player movement restricted from movement
	bool isBlocked = false;

	/** MappingContext - describes the mapping from configuration to player input for handling device input*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	
public:
	//constructor
	ATimeRewindCharacter();
	//Setup player buttons and actions
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	//Begin play after all constructors have run
	virtual void BeginPlay();

public:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to update if player has rifle - code from default FPS character */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for if player has rifle - code from default FPS character */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	UFUNCTION(BlueprintCallable, Category = Camera)
	void ActivateMainCamera(); //function to force activate FPS camera
	UFUNCTION(BlueprintCallable, Category = Camera)
	void ActivateSideCamera(); //function to force activate side camera
	UFUNCTION(BlueprintCallable, Category = Camera)
	void ActivateTopCamera(); //function to force activate top camera
	UFUNCTION(BlueprintCallable, Category = Camera)
	void ChangeCameraInput(); //function to change to next camera

	UFUNCTION(BlueprintCallable, Category = Movement)
	void UpdatePlaybackBlocking(bool shouldBlock); //function to update player movement blocking

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	//Current camera index of active camera array
	int currCameraIndex;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	/** Returns Side CameraComponent subobject **/
	UCameraComponent* GetSideCameraComponent() const { return SideCameraComponent; }
	/** Returns Top CameraComponent subobject **/
	UCameraComponent* GetTopCameraComponent() const { return TopCameraComponent; }
};

