#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CalculatePath.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UCalculatePath : public UObject
{
	GENERATED_BODY()
public:
	FVector GetClosestObjectPoint(FVector GivenLocation, AActor* Object) const;
	FVector GetFurthestObjectPoint(FVector GivenLocation, AActor* Object) const;
	double GetDistance(FVector GivenA, FVector GivenB) const;

	bool IsSameDirection(double GivenPoint, double Point, double NextPoint) const;
	bool IsValidObjectRange(FVector GivenPoint, AActor* Object) const;
	
};
