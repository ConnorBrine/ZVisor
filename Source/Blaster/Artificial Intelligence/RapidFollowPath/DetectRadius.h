#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense.h"
#include "DetectRadius.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UDetectRadius : public UAISense
{
	GENERATED_BODY()

public:
	void UpdateDetectRadius();
	
};
