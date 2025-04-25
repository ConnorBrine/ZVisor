#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense_Prediction.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/RapidFollowPath.h"
#include "UObject/NoExportTypes.h"
#include "Blaster/Objects/ObjectFaceSideType.h"
#include "PredictPossibleMove.generated.h"


#define MAX_OBJECT_FACE_RECEIVED 2
#define SHRINK_OBJECT_NUM 1
#define DEFAULT_OBJECT_SCORE 80
#define DEFAULT_REQUIRE_OBJECT_DISTANCE 2000


USTRUCT(BlueprintType)
struct FLineStruct
{
	GENERATED_BODY();
public:
	UPROPERTY(VisibleAnywhere)
	FVector LocationA = FVector();
	UPROPERTY(VisibleAnywhere)
	FVector LocationB = FVector();

};

/**
 * 
 */
UCLASS()
class BLASTER_API UPredictPossibleMove : public URapidFollowPath
{
	GENERATED_BODY()
public:
	UPredictPossibleMove();

	void SetupDefaultObject();
	void PreloadObject(AActor* Object, FVector CharacterLocation, FVector ClosestObjectLocation, FRotator CharacterRotation, FRotator ObjectRotation);
	void PreloadObjectFaceSide(AActor* Object, FVector CharacterLocation, FVector ClosestObjectLocation);
	void PreloadObjectItem(AActor* Object);
	void GenerateLocationRouteByObjects();

	void OptimizeFaceSideResult(FLineStruct GivenLeftSide, FLineStruct GivenRightSide);
	void ShrinkLocation(FLineStruct& GivenLeftSide, FLineStruct& GivenRightSide);
	void SetupLineObjectSideData(EObjectFaceSideType NameType, FVector ObjectLocation, FVector ObjectExtent, FVector CharacterLocation, FVector ClosestObjectLocation);
	void SetLineObjectSideData(EObjectFaceSideType NameType, FVector Location, FVector Extent, FLineStruct& LineObjectData);
	void LocateLineLocation(FVector GivenFirstLocation, FLineStruct& GivenLine);
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
	UPROPERTY()
	TArray<FString>RestrictScan;

	int32 NumObjectFaceReceive = 0;
	EObjectFaceSideType BestObjectFaceSide;
	double BestObjectFaceSide_Distance;
	bool InvalidTwoPointForm = false;
	FVector LastCharacterLocation;

	UPROPERTY()
	TArray<FLineStruct>LeftSideData;
	UPROPERTY()
	TArray<FLineStruct>RightSideData;
	UPROPERTY()
	TArray<FLineStruct>ResultObjectFaceSide;

	UPROPERTY()
	TArray<FVector>DistanceCloseToCharacter; //restore data of location
	UPROPERTY()
	TArray<float>ResultShortestDistance; //result data of that location

	UPROPERTY()
	TArray<float> Distance2DArray;
	int32 Distance2DSize = 0;
	//float Min2DArrayNum = 0.f;

	UPROPERTY()
	TMap<FString, FVector> ItemData;

public:
	FVector LineGenerator(FVector LocationA, FVector LocationB, double ZAxis);
	FVector TwoPointFormGenerator(FVector CharacterLocation, FVector ClosestObjectLocation, FLineStruct& ObjectSideData);
	FVector FVectorConvertRoundDouble(FVector GivenVector);

	FVector GetFinalMove();

	FLineStruct FLineStructConvertRoundDouble(FLineStruct GivenLine);
	EObjectFaceSideType GetCollisionObjectSide(FVector ObjectLocation, FVector ObjectExtent, FVector CharacterLocation, FVector ClosestObjectLocation);

	double GetDistance(FVector GivenA, FVector GivenB);
	bool IsInsideFaceSide(FVector TwoPointFormPoint, FLineStruct ObjectSideData);
	bool IsSingleLineXAxis(FVector A, FVector B);
	bool IsObjectInRange(FRotator CharacterRotation, FRotator ObjectRotation);
};
