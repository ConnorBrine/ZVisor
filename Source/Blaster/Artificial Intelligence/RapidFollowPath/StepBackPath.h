#pragma once

#include "CoreMinimal.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/RapidFollowPath.h"
#include "StepBackPath.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UAStepBackPath : public URapidFollowPath
{
	GENERATED_BODY()
public:
	void Update();
	
};
