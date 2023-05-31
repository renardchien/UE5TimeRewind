// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeRewindCharacter.h"
#include "TimeRewindProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// ATimeRewindCharacter

ATimeRewindCharacter::ATimeRewindCharacter()
{
	// Character doesnt have a rifle at start
	//this is based on the default FPS template, but is not used
	bHasRifle = false; 

	//should block character from interaction
	isBlocked = false;

	//set default camera from multiple camera positions
	currCameraIndex = 0;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	for first person camera
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true; //should follow rotation from player input

	//Create a side view camera
	SideCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideCamera"));
	SideCameraComponent->SetupAttachment(GetCapsuleComponent());
	SideCameraComponent->SetRelativeLocation(FVector(100.f, 380.f, 60.f)); // Position the camera
	SideCameraComponent->bUsePawnControlRotation = false; //should follow rotation from player input. This camera has a fixed vertical

	//Create a top down camera
	TopCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopCamera"));
	TopCameraComponent->SetupAttachment(GetCapsuleComponent());
	TopCameraComponent->SetRelativeLocation(FVector(100.f, 0, 560.f)); // Position the camera
	TopCameraComponent->bUsePawnControlRotation = false; //should follow rotation from player input. This camera has a fixed horizontal

	//Add cameras to our camera tracking array
	CameraComponentList.Add(FirstPersonCameraComponent);
	CameraComponentList.Add(SideCameraComponent);
	CameraComponentList.Add(TopCameraComponent);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true); //Only the owning player should be able to see this mesh
	Mesh1P->SetupAttachment(FirstPersonCameraComponent); //Attach mesh to camera component
	Mesh1P->bCastDynamicShadow = false; //should mesh cast a dynamic shadow
	Mesh1P->CastShadow = false; //should cast a shadow
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f)); //Set mesh location in reference to camera

}

//At beginning of player after all constructors are run
void ATimeRewindCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		//Get player input system
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			//Setup input mapping
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//Function to switch camera
void ATimeRewindCharacter::ChangeCameraInput()
{
	//Increase index to next camera in array
	currCameraIndex++;

	//if out of bounce, wrap index around in array
	if (currCameraIndex > CameraComponentList.Num() - 1)
	{
		currCameraIndex = 0;
	}

	//iterate cameras and deactivate unless new active index
	for (int i = 0; i < CameraComponentList.Num(); i++)
	{
		//activate camera if matches expected index
		if (i == currCameraIndex)
		{
			CameraComponentList[i]->SetActive(true, false);
			CameraComponentList[i]->Activate();
		}
		//deactivate camera if not active index
		else 
		{
			CameraComponentList[i]->SetActive(false);
		}
	}
}

//Force activate main FPS camera
void ATimeRewindCharacter::ActivateMainCamera()
{
	FirstPersonCameraComponent->SetActive(true, false);
	FirstPersonCameraComponent->Activate(true);
	SideCameraComponent->SetActive(false);
	TopCameraComponent->SetActive(false);
}

//Force activate side camera
void ATimeRewindCharacter::ActivateSideCamera()
{
	SideCameraComponent->SetActive(true, true);
	SideCameraComponent->Activate(true);
	FirstPersonCameraComponent->SetActive(false);
	TopCameraComponent->SetActive(false);
}

//Force activate top camera
void ATimeRewindCharacter::ActivateTopCamera()
{
	TopCameraComponent->SetActive(true, true);
	TopCameraComponent->Activate(true);
	FirstPersonCameraComponent->SetActive(false);
	SideCameraComponent->SetActive(false);
}


//////////////////////////////////////////////////////////////////////////// Input

void ATimeRewindCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATimeRewindCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATimeRewindCharacter::Look);


	}

	PlayerInputComponent->BindAction(FName("ChangeCamera"), IE_Pressed, this, &ATimeRewindCharacter::ChangeCameraInput);
}

//Player movement
void ATimeRewindCharacter::Move(const FInputActionValue& Value)
{
	//If movement is currently blocked, cancel
	if (isBlocked)
	{
		return;
	}


	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	//If controller not null
	if (Controller != nullptr)
	{
		// add movement based on input value
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

//Player camera rotation
void ATimeRewindCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	//If controller not null
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//Update if player has rifle - code from default FPS player controller
void ATimeRewindCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle; 
}

//Check if player has rifle - code from default FPS player controller
bool ATimeRewindCharacter::GetHasRifle()
{
	return bHasRifle;
}

//Should block player movement
void ATimeRewindCharacter::UpdatePlaybackBlocking(bool shouldBlock)
{
	isBlocked = shouldBlock;
}