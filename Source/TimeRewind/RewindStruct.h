//Copyright 2023 Cody Van De Mark
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RewindStruct.generated.h"

/**
 * Struct to hold position and physics state for rewind
 */
USTRUCT(BlueprintType)
struct TIMEREWIND_API FRewindStruct 
{
	GENERATED_USTRUCT_BODY()

	/* Properties all exposed to BP for use in subclasses */

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector position; //object position

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator rotation; //object rotaiton

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector angularVel; //object angular velocity

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector linearVel; //object linear velocity

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool resetPosition; //should teleport object as if respawned

	//used for optimization by putting all objects in memory at start
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isNull = true; //should this object be treated as null and skipped

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool playSound = false; //should play sound on playback
};
