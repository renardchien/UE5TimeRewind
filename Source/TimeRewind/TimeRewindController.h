//Copyright 2023 Cody Van De Mark
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "RewindStruct.h"
#include "TimeArrayStruct.h"
#include "Components/BoxComponent.h"
#include "Components/ShapeComponent.h"
#include "PhysicsChairProjectile.h"
#include "EnhancedInputComponent.h"
#include "Components/AudioComponent.h"
#include "TimeRewindCharacter.h"
#include "TimeRewindManager.h"
#include "TimeRewindController.generated.h"

/**
 * 
 */
UCLASS()
class TIMEREWIND_API ATimeRewindController : public APlayerController
{
	GENERATED_BODY()

	//max number of projectiles on screen
	const int numProjectiles = 20; 
	//spawn position within world
	const FVector projectileSpawnPosition = FVector(1650.0f, 1380.0f, -290.0f); 
	//offset from character to move projectiles to when firing
	const FVector muzzleOffset = FVector(200.0f, 0.0f, 10.0f); 
	//Projectile speed
	const float projectileSpeed = 3000.f;

public:
	//Exposed to BP to be writeable for different projectile types.
	//Used in BP_TimeRewindController
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APhysicsChairProjectile> ProjectileClass;

	//Exposed to BP to be used and edited in subclass blueprints
	UPROPERTY(VisibleDefaultsOnly, Category = Actors)
	TArray<APhysicsChairProjectile*> projectileList;

	//Exposed to BP to be writeable for different launching sounds
	//Used in BP_TimeRewindController
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound;

	//Exposed to BP to be used and edited in subclass blueprints
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATimeRewindCharacter* timeRewindCharacter;

	//Exposed to BP to be used and edited in subclass blueprints
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	ATimeRewindManager* timeRewindManager;

	//Exposed to BP to be used and edited in subclass blueprints
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UWorld* World;

	//Constructor
	ATimeRewindController();
	
	//Tracking which projectile is active
	int currProjectileIndex;

	//Function to start on play, called after all constructors complete
	void BeginPlay() override;

	//Function to fire projectile
	void FireProjectile();

	//Function to enable rewind playback exposed to BP for usage in subclasses
	//Used in BP_TimeRewindController
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void EnablePlayback(bool enablePlayback);

	//Function to paused and unpause timeline playback exposed to BP for usage in subclasses
	//Used in BP_TimeRewindController
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void PausePlayback(bool shouldPause);

	//Function to change time in playback timeline exposed to BP for usage in subclasses
	//Used in BP_TimeRewindController
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void SeekTime(float timeValue);

	//Function to check if playback is paused exposed to BP for usage in subclasses
	//Used in BP_TimeRewindController
	UFUNCTION(BlueprintCallable, Category = "Playback")
	bool GetIsPlaybackPaused();

};
