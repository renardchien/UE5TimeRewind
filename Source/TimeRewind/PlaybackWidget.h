//Copyright 2023 Cody Van De Mark
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the �Software�), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlaybackWidget.generated.h"

/**
 * 
 */
UCLASS()
class TIMEREWIND_API UPlaybackWidget : public UUserWidget
{
	GENERATED_BODY()

	void Construct();

public:
	//Function to handle click on pause button in playback UI
	//component used by PlaybackUI blueprint
	UFUNCTION(BlueprintCallable, Category = "Playback")
	void UpdatePaused(bool shouldPause);
};
