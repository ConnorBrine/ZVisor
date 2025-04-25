#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blaster/Path/RangePathType.h"
#include "CalculatePath.h"
#include "IdlePath.generated.h"

USTRUCT(BlueprintType)
struct FGeneratePath
{
	GENERATED_BODY();
public:
	double MaxX = INT_MIN;
	double MaxY = INT_MIN;
	double MinX = INT_MAX;
	double MinY = INT_MAX;
	double DefaultZ;

	double MinForwardRange = 0.f;
	double MaxForwardRange = 0.f;
	
	FVector ShortestPath; //Path that not involve with Idle Path
	int32 ShortestDistanceIndex = -1;
	int32 CurrentIndex;

	ERangePathType CurrentLocationRange;
};

UCLASS()
class BLASTER_API AIdlePath : public AActor
{
	GENERATED_BODY()
	
public:	
	AIdlePath();

	FVector GetIdlePoint(int32 const Index) const;
	int32 Num() const;
	FVector ShortestDistance(FVector GivenLocation, struct FGeneratePath PathStats);
	FVector ShortestDistanceByVertices(FVector GivenLocation, struct FGeneratePath PathStats) const;
	int32 ShortestDistancePathIndex(FVector GivenLocation, struct FGeneratePath PathStats) const;
	int32 NextIdlePathIndex(FVector GivenLocation, struct FGeneratePath PathStats) const;
	bool IsValidIdlePathRange(FVector GivenPoint, struct FGeneratePath PathVertices) const;
	FVector GeneratePathPoint(FVector VectorA, FVector VectorB, struct FGeneratePath PathStats) const;


private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI", meta=(MakeEditWidget="true", AllowPrivateAccess="true"))
	TArray<FVector>IdlePoints;

	double GetDistance(FVector GivenA, FVector GivenB) const;

	UPROPERTY(VisibleAnywhere)
	UCalculatePath* CalculatePath;

	FGeneratePath AssignRangePath(double ValueMin, double ValueMax, struct FGeneratePath PathStats) const;
	bool IsSameDirection(double GivenPoint, double Point, double NextPoint) const;
};
