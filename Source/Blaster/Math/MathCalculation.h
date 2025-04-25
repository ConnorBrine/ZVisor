#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MathCalculation.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UMathCalculation : public UObject
{
	GENERATED_BODY()
public: 
	static double GetDistance(FVector GivenA, FVector GivenB);
	
};
