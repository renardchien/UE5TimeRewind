//Copyright 2023 Cody Van De Mark
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "TimeRewindManager.h"
#include "TimeRewindCharacter.h"
#include "PhysicsTimeActor.h"
#include "TimeArrayStruct.h"

// Sets default values
ATimeRewindManager::ATimeRewindManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Determine number of positions in timeline arrays
	numPositions = TimeRecorded / TimeDelay;

}

// Called when the game starts or when spawned
void ATimeRewindManager::BeginPlay()
{
	//Call parent begin play
	Super::BeginPlay();

	//Get ref to world object
	World = GetWorld();

	//get player character reference
	ACharacter* charRef = UGameplayStatics::GetPlayerCharacter(this, 0);
	timeRewindCharacter = Cast<ATimeRewindCharacter>(charRef);

	//Array to grab all actors with physics item tag to track in timeline
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsWithTag(this, FName("PhysicsItem"), foundActors);

	//for each found actor add to array of collisions objects and add to tracking map for timeline
	for (AActor* foundActor : foundActors)
	{
		//get collision component from actor
		APhysicsTimeActor* physicsObj = Cast<APhysicsTimeActor>(foundActor);
		UActorComponent* foundComponent = physicsObj->GetComponentByClass(UShapeComponent::StaticClass());
		UShapeComponent* boxCollision = Cast<UShapeComponent>(foundComponent);

		//add collision object to array of objects to track
		physicsObjList.Add(boxCollision);

		//add collision object to make of timeline positions for object
		positionObjMap.Add(boxCollision, FTimeArrayStruct(numPositions));
	}

	// Call RepeatingFunction once per second, starting zero seconds from now.
	GetWorldTimerManager().SetTimer(rewindTimer, this, &ATimeRewindManager::UpdateRewind, TimeDelay, true, 0.0f);
}

//Tick every few milliseconds
void ATimeRewindManager::Tick(float DeltaTime)
{
	//call super class tick
	Super::Tick(DeltaTime);

	//skip if not playback mode
	if (!isInPlayback) 
	{
		return;
	}

	//Update rewind position in playback mode with updated interpolation
	UpdatePlaybackPositions(DeltaTime);
}

//Replays objects to specific positions on timeline based on delta time
void ATimeRewindManager::UpdatePlaybackPositions(float deltaTime)
{
	//calculate delta for interpolation
	deltaOverTime += (deltaTime * animationSpeed);
	//cap interpolation to 1 so we don't over interpolate
	deltaOverTime = deltaOverTime > 1 ? 1.0f : deltaOverTime;

	//determine if should play a sound effect on playback
	bool shouldPlaySound = false;

	//For each object we are tracking
	for (UShapeComponent* physicsObj : physicsObjList)
	{
		//Find the timeline struct for this collision object
		FTimeArrayStruct* foundStruct = positionObjMap.Find(physicsObj);

		//if not found, skip this
		if (foundStruct == nullptr)
		{
			continue;
		}

		//check if we have timeline position for this index
		if (foundStruct->TimePoints.IsValidIndex(currPlaybackIndex))
		{
			//get recorded struct at time position
			FRewindStruct currRewindStruct = foundStruct->TimePoints[currPlaybackIndex];

			//if this struct is flagged as null, skip this
			if (currRewindStruct.isNull == true)
			{
				continue;
			}

			//is this considered a manually reset timeline position
			//if so we will just teleport the object to the location without animation
			if (currRewindStruct.resetPosition)
			{
				//manually teleport position and physics state
				physicsObj->SetWorldLocation(currRewindStruct.position, false);
				physicsObj->SetWorldRotation(currRewindStruct.rotation, false);
				physicsObj->SetPhysicsLinearVelocity(currRewindStruct.linearVel, false);
				physicsObj->SetPhysicsAngularVelocityInRadians(currRewindStruct.angularVel, false);
			}
			//if it is not a reset, interpolate and animate positions
			else
			{
				//Interpolate positions and physics state over time to animate
				FVector newPos = FMath::Lerp(physicsObj->GetComponentLocation(), currRewindStruct.position, deltaOverTime);
				FRotator newRot = FMath::Lerp(physicsObj->GetComponentRotation(), currRewindStruct.rotation, deltaOverTime);
				FVector newLinearVel = FMath::Lerp(physicsObj->GetPhysicsLinearVelocity(), currRewindStruct.linearVel, deltaOverTime);
				FVector newAngularVel = FMath::Lerp(physicsObj->GetPhysicsAngularVelocityInRadians(), currRewindStruct.angularVel, deltaOverTime);

				//Update positions and physics state to interpolated positions
				physicsObj->SetWorldLocation(newPos, false);
				physicsObj->SetWorldRotation(newRot, false);
				physicsObj->SetPhysicsLinearVelocity(newLinearVel, false);
				physicsObj->SetPhysicsAngularVelocityInRadians(newAngularVel, false);
			}

			//If this object had a sound, flag sound for playback
			if (currRewindStruct.playSound)
			{
				shouldPlaySound = true;
			}
		}
	}

	//if fire sound is set and we flagged this animation for playback, play sound once
	//Play sound only once, otherwise it will sound very distorted all sounds play at the same time in playback
	if (shouldPlaySound && FireSound != nullptr)
	{
		//do not play a sound if sound is already playing
		if (currentAudioPlayback == nullptr || currentAudioPlayback->IsPlaying() == false)
		{
			//uncomment if you want sound playback
			//currentAudioPlayback = UGameplayStatics::SpawnSoundAtLocation(this, FireSound, timeRewindCharacter->GetActorLocation());
		}
	}

}

//Update and adds timeline positions
void ATimeRewindManager::UpdateRewind()
{
	//If game is paused or currently in playback mode, don't record anything
	if (World->IsPaused() || isInPlayback)
	{
		//if timer is not set to loop, reset it
		return;
	}

	//for each collision object we are tracking, record its current position and physics state
	for (UShapeComponent* physicsObj : physicsObjList)
	{
		AddTimelinePosition(physicsObj);
	}

	//increase timeline position
	currPosition++;
}

//Add a collision object to track in rewind timeline
void ATimeRewindManager::AppendPhysicsObject(UShapeComponent* physicsObj)
{
	//add collision object to tracking array and timeline map
	physicsObjList.Add(physicsObj);
	positionObjMap.Add(physicsObj, FTimeArrayStruct(numPositions));
}

//Remove a collision object from tracking in rewind timeline
void ATimeRewindManager::RemovePhysicsObject(UShapeComponent* physicsObjToRemove)
{
	physicsObjList.Remove(physicsObjToRemove);
	positionObjMap.Remove(physicsObjToRemove);
}

//Manually add object update position to the timeline
void ATimeRewindManager::ManuallyUpdateTimelineObject(UShapeComponent* physicsObj)
{
	//find timeline struct for this collision object
	FTimeArrayStruct* foundStruct = positionObjMap.Find(physicsObj);

	//Make sure the position before the current position is valid
	//We will inject a position before the latest in the timeline so we need to make sure that is valid
	if (currPosition - 1 < 0)
	{
		return;
	}

	//get object data to record
	FRewindStruct newStruct = FRewindStruct();
	newStruct.position = physicsObj->GetComponentLocation();
	newStruct.rotation = physicsObj->GetComponentRotation();
	newStruct.linearVel = physicsObj->GetPhysicsLinearVelocity();
	newStruct.angularVel = physicsObj->GetPhysicsAngularVelocityInRadians();
	//mark as a manual reset
	newStruct.resetPosition = true;
	//this is a valid position so mark as not null
	newStruct.isNull = false;
	//mark to play sound (could be disabled)
	newStruct.playSound = true;

	//Insert this into previous position in array
	//We are injecting a manual position and then letting normal update occur
	//Otherwise normal update would override this
	foundStruct->TimePoints[currPosition - 1] = newStruct;
}

//Adds a new timeline position for a specific collision object
void ATimeRewindManager::AddTimelinePosition(UShapeComponent* physicsObj)
{
	//find timeline array struct for this collision object
	FTimeArrayStruct* foundStruct = positionObjMap.Find(physicsObj);

	//If current tracking position out of range, remove the oldest and add to end
	if (currPosition > numPositions - 1)
	{
		foundStruct->TimePoints.RemoveAt(0, 1, true);
		foundStruct->TimePoints.Add(FRewindStruct());

		//decrement position back to valid end of array
		currPosition--;
	}

	//create new timeline object of current position and physics
	FRewindStruct newStruct = FRewindStruct();
	newStruct.position = physicsObj->GetComponentLocation();
	newStruct.rotation = physicsObj->GetComponentRotation();
	newStruct.linearVel = physicsObj->GetPhysicsLinearVelocity();
	newStruct.angularVel = physicsObj->GetPhysicsAngularVelocityInRadians();
	//not a reset, should be used for interpolation
	newStruct.resetPosition = false;
	//mark as a valid array postion and should be used for timeline
	newStruct.isNull = false;
	//should not play sound since this is automated and not an event / user action
	newStruct.playSound = false;

	//add object to timeline
	foundStruct->TimePoints[currPosition] = newStruct;
}

//Check if currently in timeline playback mode
bool ATimeRewindManager::GetIsInPlayback()
{
	return isInPlayback;
}

//Check if playback is currently paused or playing
bool ATimeRewindManager::GetIsPlaybackPaused()
{
	return isPlaybackPaused;
}

//Toggle timeline playback mode
void ATimeRewindManager::EnablePlayback(bool enablePlayback)
{
	//update blocking status for character controls depending on if we are in playback mode
	timeRewindCharacter->UpdatePlaybackBlocking(enablePlayback);

	//create new rewind struct for rewind feature
	FRewindStruct currStruct = FRewindStruct();

	//for each collision object we are tracking
	for (UShapeComponent* physicsObj : physicsObjList)
	{
		//if object has been destroyed, remove it from tracking
		if (physicsObj == nullptr) 
		{
			physicsObjList.Remove(physicsObj);
			positionObjMap.Remove(physicsObj);
			continue;
		}

		//enable or disable physics based on whether we are in playback mode
		physicsObj->SetSimulatePhysics(!enablePlayback);

		//find timeline struct for this collision object
		FTimeArrayStruct* foundStruct = positionObjMap.Find(physicsObj);

		//if we are enabling playback mode
		if (enablePlayback)
		{
			//Since we just started playback mode, rewind to 0 position and 
			//set objects to their oldest timeline value
			if (foundStruct->TimePoints.IsValidIndex(0))
			{
				currStruct = foundStruct->TimePoints[0];

				physicsObj->SetWorldLocation(currStruct.position, false);
				physicsObj->SetWorldRotation(currStruct.rotation, false);
				physicsObj->SetPhysicsLinearVelocity(currStruct.linearVel, false);
				physicsObj->SetPhysicsAngularVelocityInRadians(currStruct.angularVel, false);
			}
		}
		//if we are disabling playback mode
		else
		{
			//get the latest position the player got to in playback mode
			currStruct = foundStruct->TimePoints[currPlaybackIndex];

			//update all of the objects to this specific position in timeline before unpausing
			for (int i = currPlaybackIndex; i < foundStruct->TimePoints.Num(); i++)
			{
				if (foundStruct->TimePoints.IsValidIndex(i))
				{
					foundStruct->TimePoints[i].isNull = true;
				}
			}
		}
	}

	//update playback tracking
	isInPlayback = enablePlayback;

	//update latest index to specific position from timeline before pausing/unpausing
	currPosition = currPlaybackIndex;
	//reset current playback index
	currPlaybackIndex = 0;
}

//Toggle playback paused or playing
void ATimeRewindManager::PausePlayback(bool shouldPause)
{
	isPlaybackPaused = shouldPause;
}

//Update time in playback timeline
void ATimeRewindManager::SeekTime(float timeValue)
{
	//time value is between 0 - 30 (or max)
	float positionTime = timeValue / TimeRecorded;

	//calculate array position based off of the position time and number of positions
	int arrayPos = int(positionTime * numPositions);

	//if valid index
	if (physicsObjList.IsValidIndex(0))
	{
		//pull timeline struct for first object in array
		FTimeArrayStruct* foundStruct = positionObjMap.Find(physicsObjList[0]);

		//check if the current array pos is a valid place in our timelines
		if (foundStruct->TimePoints.IsValidIndex(arrayPos))
		{
			//set our current playback index to the calculated seek position
			currPlaybackIndex = arrayPos;
			//reset delta time for interpolation
			deltaOverTime = 0.0f;
		}
	}
}
