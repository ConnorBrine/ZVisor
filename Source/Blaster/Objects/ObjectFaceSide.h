#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blaster/Objects/ObjectFaceSideType.h"
#include "ObjectFaceSide.generated.h"

#define MAX_OBJECT_FACE_RECEIVED_DEBUG 2
#define SHRINK_OBJECT_NUM_DEBUG 1
#define DEFAULT_OBJECT_SCORE_DEBUG 80
#define DEFAULT_REQUIRE_OBJECT_DISTANCE_DEBUG 2000


USTRUCT(BlueprintType)
struct FLineStructDebug
{
	GENERATED_BODY();
public:
	FVector LocationA;
	FVector LocationB;

};

//Location A closer that Location B from the Player Location
UCLASS()
class BLASTER_API UObjectFaceSide : public UObject
{
	GENERATED_BODY()
public:
	UObjectFaceSide();

	void SetupDefaultObject();
	void PreloadObject(AActor* Object, FVector CharacterLocation, FVector ClosestObjectLocation, FRotator CharacterRotation, FRotator ObjectRotation);
	void PreloadObjectFaceSide(AActor* Object, FVector CharacterLocation, FVector ClosestObjectLocation);
	void PreloadObjectItem(AActor* Object);
	void GenerateLocationRouteByObjects();

	void OptimizeFaceSideResult(FLineStructDebug GivenLeftSide, FLineStructDebug GivenRightSide);
	void ShrinkLocation(FLineStructDebug& GivenLeftSide, FLineStructDebug& GivenRightSide);
	void SetupLineObjectSideData(EObjectFaceSideType NameType, FVector ObjectLocation, FVector ObjectExtent, FVector CharacterLocation, FVector ClosestObjectLocation);
	void SetLineObjectSideData(EObjectFaceSideType NameType, FVector Location, FVector Extent, FLineStructDebug& LineObjectData);
	void LocateLineLocation(FVector GivenFirstLocation, FLineStructDebug& GivenLine);
	void GetClosestDistancePointFromAvailablePoint();
	void GetPickupScore();

	void ConvertToPercent();

	void AutoPredictPossibleMoveFromLatestData();

	void PrintAllFaceSide();
	void PrintAllResultPoint();
	void PrintAllDistancePoint();
	void PrintAllDistancePointPercent();

	void Clear();

	//Dijkstra
	void ReloadDistance2D();
	TArray<float> CheckDistanceByDijkstra();
	int32 MinDistanceDijkstra(TArray<float>PointResult, TArray<bool>PointCheck);
	void PrintShortestValueFromDijkstra(TArray<float>PointResult);
	void ApplyDistance2DToWorld();

private:
	TArray<FString>RestrictScan;
	TArray<FString>ObjectScan;

	int32 NumObjectFaceReceive = 0;
	EObjectFaceSideType BestObjectFaceSide;
	double BestObjectFaceSide_Distance;
	bool InvalidTwoPointForm = false;
	FVector LastCharacterLocation;

	TArray<FLineStructDebug>LeftSideData;
	TArray<FLineStructDebug>RightSideData;
	TArray<FLineStructDebug>ResultObjectFaceSide;

	TArray<FVector>DistanceCloseToCharacter; //restore data of location
	TArray<float>ResultShortestDistance; //result data of that location
	TArray<int32>PickupScore;

	TArray<float> Distance2DArray;
	int32 Distance2DSize = 0;
	//float Min2DArrayNum = 0.f;

	TMap<FString, FVector> ItemData;

public:
	FVector LineGenerator(FVector LocationA, FVector LocationB, double ZAxis);
	FVector TwoPointFormGenerator(FVector CharacterLocation, FVector ClosestObjectLocation, FLineStructDebug& ObjectSideData);
	FVector FVectorConvertRoundDouble(FVector GivenVector);

	FLineStructDebug FLineStructConvertRoundDouble(FLineStructDebug GivenLine);
	EObjectFaceSideType GetCollisionObjectSide(FVector ObjectLocation, FVector ObjectExtent, FVector CharacterLocation, FVector ClosestObjectLocation);

	double GetDistance(FVector GivenA, FVector GivenB);
	bool IsInsideFaceSide(FVector TwoPointFormPoint, FLineStructDebug ObjectSideData);
	bool IsSingleLineXAxis(FVector A, FVector B);
	bool IsObjectInRange(FRotator CharacterRotation, FRotator ObjectRotation);
};
