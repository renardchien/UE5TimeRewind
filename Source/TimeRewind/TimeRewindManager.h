//Copyright 2023 Cody Van De Mark
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once


#include "RewindStruct.h"
#include "TimeArrayStruct.h"
#include "TimeRewindCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/ShapeComponent.h"
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TimeRewindManager.generated.h"

UCLASS()
class TIMEREWIND_API ATimeRewindManager : public AActor
{
	GENERATED_BODY()

	//delay between each event recording
	const float TimeDelay = 0.06f; //seconds
	//max time to record events
	const float TimeRecorded = 30.0f; //seconds
	//rate at which to accelerate interpolation
	const float animationSpeed = 4.0f;
	//number of array positions in time recording
	int numPositions;
	//current playback index in array of time recording
	int currPlaybackIndex;
	//delta for animation interpolation
	float deltaOverTime;
	
public:	
	//Constructor
	ATimeRewindManager();

	//Maps collision object to struct of recorded positions for these positions
	//Exposed to blueprint for use in subclasses
	UPROPERTY(VisibleDefaultsOnly, Category = Time)
	TMap<UShapeComponent*, FTimeArrayStruct> positionObjMap;

	//If in playback mode
	//Exposed to blueprint for use in subclasses
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	bool isInPlayback = false;

	//Array of collision objects tracked for time rewind
	//Exposed to blueprint for use in subclasses
	UPROPERTY(VisibleDefaultsOnly, Category = Actors)
	TArray<UShapeComponent*> physicsObjList;

	//Timer to update and track rewinding
	//Exposed to blueprint for use in subclasses
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	FTimerHandle rewindTimer;

	//Fire sound effect file exposed to blueprint to be set
	//Used in BP_TimeRewindManager
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* FireSound;

	//Current position to add new timeline events to
	//Exposed to blueprint for use in subclasses
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int currPosition;

	//Reference to character pawn
	//Exposed to blueprint for use in subclasses
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	ATimeRewindCharacter* timeRewindCharacter;

	//Is the playback paused or playing
	//Exposed to blueprint for use in subclasses
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
	bool isPlaybackPaused = true;

	//Reference to game world object
	//Exposed to blueprint for use in subclasses
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	UWorld* World;

	//Audio playback reference hidden internally from BP
	UAudioComponent* currentAudioPlayback;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Update and adds timeline positions
	void UpdateRewind();
	//Adds a new timeline position for a specific collision object
	void AddTimelinePosition(UShapeComponent* physicsObj);
	//Replays objects to specific positions on timeline based on delta time
	void UpdatePlaybackPositions(float deltaTime);

	//Check if currently in timeline playback mode
	//Exposed to blueprint for use in subclasses
	UFUNCTION(BlueprintCallable, Category = "Playback")
	bool GetIsInPlayback();

	//Check if playback is currently paused or playing
	//Exposed to blueprint for use in subclasses
	UFUNCTION(BlueprintCallable, Category = "Playback")
	bool GetIsPlaybackPaused();

	//Toggle timeline playback mode
	//Exposed to blueprint for use in subclasses
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void EnablePlayback(bool enablePlayback);

	//Toggle playback paused or playing
	//Exposed to blueprint for use in subclasses
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void PausePlayback(bool shouldPause);

	//Update time in playback timeline
	//Exposed to blueprint for use in subclasses
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void SeekTime(float timeValue);

	//Manually add object update position to the timeline
	//Exposed to blueprint for use in subclasses
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void ManuallyUpdateTimelineObject(UShapeComponent* physicsObj);

	//Add a collision object to track in rewind timeline
	//Exposed to blueprint for use in subclasses
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void AppendPhysicsObject(UShapeComponent* physicsObj);

	//Remove a collision object from tracking in rewind timeline
	//Exposed to blueprint for use in subclasses
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void RemovePhysicsObject(UShapeComponent* physicsObjToRemove);
};
