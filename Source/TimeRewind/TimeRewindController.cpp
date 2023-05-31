//Copyright 2023 Cody Van De Mark
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "TimeRewindController.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimeRewindCharacter.h"
#include "PhysicsTimeActor.h"
#include "TimeArrayStruct.h"
#include "GameFramework/ProjectileMovementComponent.h"


//Constructor
ATimeRewindController::ATimeRewindController()
{
	//Set default values
	currProjectileIndex = 0;
}

//Call to start after game behinds
void ATimeRewindController::BeginPlay()
{
	//Call parent function
	Super::BeginPlay();

	//Get player character ref
	ACharacter* charRef = UGameplayStatics::GetPlayerCharacter(this, 0);
	timeRewindCharacter = Cast<ATimeRewindCharacter>(charRef);

	//Find rewind manager in scene. This must be placed in the game scene or as a child of an actor to be found here
	//In this example case, the object is in the scene
	AActor* foundManager = UGameplayStatics::GetActorOfClass(this, ATimeRewindManager::StaticClass());
	timeRewindManager = Cast<ATimeRewindManager>(foundManager);

	//Get ref to world
	World = GetWorld();

	//Set camera to time rewind character
	SetViewTarget(timeRewindCharacter);

	//Find fire key to fire projectile function
	InputComponent->BindAction(FName("Fire"), IE_Pressed, this, &ATimeRewindController::FireProjectile);

	//if projectile and world objects defined, spawn default projectiles
	if (ProjectileClass != nullptr && World != nullptr)
	{
		//pre-spawn max projectiles offscreen
		for (int i = 0; i < numProjectiles; i++)
		{
			//Set spawn conditions to always spawn 
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			//Spawn actor offscreen
			APhysicsChairProjectile* newProjectile = World->SpawnActor<APhysicsChairProjectile>(ProjectileClass, projectileSpawnPosition, FRotator::ZeroRotator, ActorSpawnParams);

			//Find collision component on projectile
			UActorComponent* foundComponent = newProjectile->GetComponentByClass(UBoxComponent::StaticClass());
			UBoxComponent* boxCollision = Cast<UBoxComponent>(foundComponent);
			//disable physics on collision component
			boxCollision->SetSimulatePhysics(false);

			//Add to pre-spawned projectile list
			projectileList.Add(newProjectile);

			//if rewind manager exists, add collision object to tracked object list
			if (timeRewindManager != nullptr)
			{
				timeRewindManager->AppendPhysicsObject(boxCollision);
			}
		}
	}
}

//Function to fire projectile
void ATimeRewindController::FireProjectile()
{
	//if in playback timeline or missing manager, cancel
	if (timeRewindManager != nullptr && timeRewindManager->GetIsInPlayback())
	{
		return;
	}

	//Find player rotation
	const FRotator SpawnRotation = PlayerCameraManager->GetCameraRotation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector ProjectileSpawnLocation = timeRewindCharacter->GetActorLocation() + SpawnRotation.RotateVector(muzzleOffset);

	//Check if our current projectile index from pre-spawned projectiles is out of range and wrap around
	if (currProjectileIndex >= projectileList.Num() - 1)
	{
		currProjectileIndex = 0; //reset projectile index
	}

	//Get current projectile
	APhysicsChairProjectile* chairProjectile = projectileList[currProjectileIndex];

	//Grab collision component from projectile
	UActorComponent* foundComponent = chairProjectile->GetComponentByClass(UShapeComponent::StaticClass());
	UShapeComponent* boxCollision = Cast<UShapeComponent>(foundComponent);

	//enable physics on projectile
	boxCollision->SetSimulatePhysics(true);

	//Determined forward direction from character normalized so we can add our own force
	FVector direction = timeRewindCharacter->GetMesh1P()->GetForwardVector().GetSafeNormal();

	//scale direction by projectile speed and set as linear velocity
	boxCollision->SetPhysicsLinearVelocity(direction * projectileSpeed);

	//Get movement component from projectile
	UActorComponent* foundMovementComponent = chairProjectile->GetComponentByClass(UProjectileMovementComponent::StaticClass());
	UProjectileMovementComponent* ProjectileMovement = Cast<UProjectileMovementComponent>(foundMovementComponent);

	//Set projectile velocity properties on movement component
	ProjectileMovement->InitialSpeed = projectileSpeed;
	ProjectileMovement->MaxSpeed = projectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	//Set projectile location and rotation
	chairProjectile->SetActorLocationAndRotation(ProjectileSpawnLocation, SpawnRotation);

	//increase projectile index for next shot
	currProjectileIndex++;

	//if sound file is selected, play sound at player location
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, timeRewindCharacter->GetActorLocation());
	}

	//If time rewind manager, manually update the timeline with a reset position as if a new projectile spawn
	if (timeRewindManager != nullptr)
	{
		timeRewindManager->ManuallyUpdateTimelineObject(boxCollision);
	}
}


//Function to enable rewind playback exposed to BP for usage in subclasses
void ATimeRewindController::EnablePlayback(bool enablePlayback)
{
	if (timeRewindManager != nullptr)
	{
		timeRewindManager->EnablePlayback(enablePlayback);
	}
}

//Function to paused and unpause timeline playback exposed to BP for usage in subclasses
void ATimeRewindController::PausePlayback(bool shouldPause)
{
	if (timeRewindManager != nullptr)
	{
		timeRewindManager->PausePlayback(shouldPause);
	}
}

//Function to change time in playback timeline exposed to BP for usage in subclasses
void ATimeRewindController::SeekTime(float timeValue)
{
	if (timeRewindManager != nullptr)
	{
		timeRewindManager->SeekTime(timeValue);
	}
}

//Function to check if playback is paused exposed to BP for usage in subclasses
bool ATimeRewindController::GetIsPlaybackPaused()
{
	if (timeRewindManager != nullptr)
	{
		return timeRewindManager->GetIsPlaybackPaused();
	}

	return false;
}


