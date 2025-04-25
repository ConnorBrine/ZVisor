#include "ObjectFaceSide.h"
#include "ObjectFaceSideType.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Pickup/Pickup.h"
#include "Blaster/Pickup/PathChapterItemPickup.h"
#include "Kismet/KismetSystemLibrary.h"

UObjectFaceSide::UObjectFaceSide()
{
	SetupDefaultObject();
}

void UObjectFaceSide::SetupDefaultObject()
{
	RestrictScan.Add("LevelSequenceActor");
	RestrictScan.Add("Floor");
	RestrictScan.Add("RecastNavMesh");
	RestrictScan.Add("SkyAtmosphere");
	RestrictScan.Add("BP"); //current all Blueprint will locked
	RestrictScan.Add("NavMeshBoundsVolume");
	//RestrictScan.Add("Cube");
	RestrictScan.Add("Plane");
	RestrictScan.Add("PathChapter");
	RestrictScan.Add("Light");
	RestrictScan.Add("Notification");
}
/*
* This default recommend for running Predict Possible Move
*/
void UObjectFaceSide::AutoPredictPossibleMoveFromLatestData()
{
	GenerateLocationRouteByObjects();
	ReloadDistance2D();
	ConvertToPercent();
	GetPickupScore();
	PrintAllDistancePointPercent();
}

void UObjectFaceSide::PreloadObject(AActor* Object, FVector CharacterLocation, FVector ClosestObjectLocation, FRotator CharacterRotation, FRotator ObjectRotation)
{
	//if (IsObjectInRange(CharacterRotation, ObjectRotation)) return;
	if (Object->GetClass()->IsChildOf(AWeapon::StaticClass()) ||
		Object->GetClass()->IsChildOf(APickup::StaticClass()))
	{
		if (Object->GetClass()->IsChildOf(APathChapterItemPickup::StaticClass())) return;
		PreloadObjectItem(Object);
		return;
	}
	FString ObjectClassName = Object->GetClass()->GetName();

	for (FString Restrict : RestrictScan)
	{
		UE_LOG(LogTemp, Error, TEXT("%s and %s"), *ObjectClassName, *Restrict);
		if (ObjectClassName.Contains(Restrict))
		{
			UE_LOG(LogTemp, Error, TEXT("%s restrict"), *ObjectClassName);
			return;
		}
	}
	PreloadObjectFaceSide(Object, CharacterLocation, ClosestObjectLocation);

}

void UObjectFaceSide::PreloadObjectFaceSide(AActor* Object, FVector CharacterLocation, FVector ClosestObjectLocation)
{
	FVector Origin;
	FVector ObjectExtent;

	FLineStructDebug SideLeft;
	FLineStructDebug SideRight;

	LastCharacterLocation = CharacterLocation;

	BestObjectFaceSide = EObjectFaceSideType::EOFST_MAX;

	UKismetSystemLibrary::GetActorBounds(Object, Origin, ObjectExtent);

	GetCollisionObjectSide(Origin, ObjectExtent, CharacterLocation, ClosestObjectLocation);

	if (BestObjectFaceSide == EObjectFaceSideType::EOFST_MAX)
	{
		UE_LOG(LogTemp, Warning, TEXT("==> Nothing"));
	}
	switch (BestObjectFaceSide)
	{
	case EObjectFaceSideType::EOFST_UP:
	case EObjectFaceSideType::EOFST_DOWN:
		SetLineObjectSideData(EObjectFaceSideType::EOFST_LEFT, Origin, ObjectExtent, SideLeft);
		SetLineObjectSideData(EObjectFaceSideType::EOFST_RIGHT, Origin, ObjectExtent, SideRight);
		UE_LOG(LogTemp, Warning, TEXT("==> LeftRight"));
		break;

	case EObjectFaceSideType::EOFST_LEFT:
	case EObjectFaceSideType::EOFST_RIGHT:
		SetLineObjectSideData(EObjectFaceSideType::EOFST_UP, Origin, ObjectExtent, SideLeft);
		SetLineObjectSideData(EObjectFaceSideType::EOFST_DOWN, Origin, ObjectExtent, SideRight);
		UE_LOG(LogTemp, Warning, TEXT("==> UpDown"));
		break;
	}
	//DrawDebugLine(GetWorld(), SideLeft.LocationA, SideLeft.LocationB, FColor::Yellow, true);
	//DrawDebugLine(GetWorld(), SideRight.LocationA, SideRight.LocationB, FColor::Yellow, true);

	LocateLineLocation(CharacterLocation, SideLeft);
	LocateLineLocation(CharacterLocation, SideRight);

	//OptimizeFaceSideResult(SideLeft, SideRight);
	LeftSideData.Add(SideLeft);
	RightSideData.Add(SideRight);


}

void UObjectFaceSide::PreloadObjectItem(AActor* Object)
{
	FVector Origin;
	FVector ObjectExtent; //wont need that
	UKismetSystemLibrary::GetActorBounds(Object, Origin, ObjectExtent);

	UE_LOG(LogTemp, Warning, TEXT("PPM Object Name: %s"), *Object->GetName());
	ItemData.Emplace(Object->GetName(), Origin);
}

void UObjectFaceSide::GenerateLocationRouteByObjects()
{
	for (int i = 0; i < LeftSideData.Num(); i++)
	{
		OptimizeFaceSideResult(LeftSideData[i], RightSideData[i]);
	}
}

void UObjectFaceSide::OptimizeFaceSideResult(FLineStructDebug GivenLeftSide, FLineStructDebug GivenRightSide)
{
	FLineStructDebug LatestObjectFaceSide;

	FHitResult CurrentLineCollision;
	ShrinkLocation(GivenLeftSide, GivenRightSide);

	//DrawDebugLine(GetWorld(), GivenLeftSide.LocationA, GivenLeftSide.LocationB, FColor::Yellow, true);
	//DrawDebugLine(GetWorld(), GivenRightSide.LocationA, GivenRightSide.LocationB, FColor::Yellow, true);

	GetWorld()->LineTraceSingleByChannel(
		CurrentLineCollision,
		GivenLeftSide.LocationB,
		GivenLeftSide.LocationA,
		ECollisionChannel::ECC_Visibility
	);
	LatestObjectFaceSide.LocationA = CurrentLineCollision.ImpactPoint == FVector(0) ?
		GivenLeftSide.LocationB : CurrentLineCollision.ImpactPoint;

	GetWorld()->LineTraceSingleByChannel(
		CurrentLineCollision,
		GivenRightSide.LocationB,
		GivenRightSide.LocationA,
		ECollisionChannel::ECC_Visibility
	);
	LatestObjectFaceSide.LocationB = CurrentLineCollision.ImpactPoint == FVector(0) ?
		GivenRightSide.LocationB : CurrentLineCollision.ImpactPoint;

	ResultObjectFaceSide.Add(LatestObjectFaceSide);
}

void UObjectFaceSide::ShrinkLocation(FLineStructDebug& GivenLeftSide, FLineStructDebug& GivenRightSide)
{
	switch (BestObjectFaceSide)
	{
	case EObjectFaceSideType::EOFST_UP:
	case EObjectFaceSideType::EOFST_DOWN:
		GivenLeftSide.LocationA.X += SHRINK_OBJECT_NUM_DEBUG;
		GivenLeftSide.LocationB.X += SHRINK_OBJECT_NUM_DEBUG;
		GivenRightSide.LocationA.X -= SHRINK_OBJECT_NUM_DEBUG;
		GivenRightSide.LocationB.X -= SHRINK_OBJECT_NUM_DEBUG;
		break;

	case EObjectFaceSideType::EOFST_LEFT:
	case EObjectFaceSideType::EOFST_RIGHT:
		GivenLeftSide.LocationA.Y -= SHRINK_OBJECT_NUM_DEBUG;
		GivenLeftSide.LocationB.Y -= SHRINK_OBJECT_NUM_DEBUG;
		GivenRightSide.LocationA.Y += SHRINK_OBJECT_NUM_DEBUG;
		GivenRightSide.LocationB.Y += SHRINK_OBJECT_NUM_DEBUG;
		break;

	}

}

void UObjectFaceSide::LocateLineLocation(FVector GivenFirstLocation, FLineStructDebug& GivenLine)
{
	if (GetDistance(GivenFirstLocation, GivenLine.LocationA) > GetDistance(GivenFirstLocation, GivenLine.LocationB))
	{
		FVector TempLocation = GivenLine.LocationA;
		GivenLine.LocationA = GivenLine.LocationB;
		GivenLine.LocationB = TempLocation;
	}
	//else nothing change
}

void UObjectFaceSide::GetClosestDistancePointFromAvailablePoint()
{

}

/*
* If using this function, this will following after
* ApplyDistance2DToWorld (Dijkstra)
*/
void UObjectFaceSide::GetPickupScore()
{
	int32 ObjMultiply;
	for (int32 i = 0; i < ResultShortestDistance.Num(); i++)
	{
		ObjMultiply = 0;
		for (TPair<FString, FVector> ObjPair : ItemData)
		{
			if ((ObjPair.Value - DistanceCloseToCharacter[i]).Size() <= DEFAULT_REQUIRE_OBJECT_DISTANCE_DEBUG)
			{
				++ObjMultiply;
			}
		}

		
		float ItemScore = (float)(DEFAULT_OBJECT_SCORE_DEBUG * ObjMultiply) / (ItemData.IsEmpty() ? 1 : ItemData.Num());
		UE_LOG(LogTemp, Error, TEXT("This pickup score: %d [before = %f] ItemScore = %f"), ItemData.Num(), ResultShortestDistance[i], ItemScore);
		ResultShortestDistance[i] = (float)(ResultShortestDistance[i] + ItemScore) / 2;
		UE_LOG(LogTemp, Error, TEXT("This pickup score: [after = %f]"), ResultShortestDistance[i])
	}
}

void UObjectFaceSide::ConvertToPercent()
{
	TArray<float> MaxIndex = ResultShortestDistance;
	
	if (!MaxIndex.IsEmpty())
	{
		MaxIndex.Sort();
		float MaxNum = MaxIndex[MaxIndex.Num() - 1];
		UE_LOG(LogTemp, Warning, TEXT("Maximum for PPM: %f"), MaxNum);

		for (int32 Index = 0; Index < ResultShortestDistance.Num(); Index++)
		{
			ResultShortestDistance[Index] = 100 - (100 * ((float)ResultShortestDistance[Index] / MaxNum));
		}
	}
}



void UObjectFaceSide::PrintAllFaceSide()
{
	for (int i = 0; i < LeftSideData.Num(); i++)
	{
		DrawDebugLine(GetWorld(), LeftSideData[i].LocationA, LeftSideData[i].LocationB, FColor::Magenta, true);
		DrawDebugLine(GetWorld(), RightSideData[i].LocationA, RightSideData[i].LocationB, FColor::Magenta, true);
	}
}

void UObjectFaceSide::PrintAllResultPoint()
{
	for (int i = 0; i < LeftSideData.Num(); i++)
	{
		DrawDebugSphere(GetWorld(), ResultObjectFaceSide[i].LocationA, 12.f, 12, FColor::Orange, true);
		DrawDebugString(GetWorld(), ResultObjectFaceSide[i].LocationA, "LocA");
		DrawDebugSphere(GetWorld(), ResultObjectFaceSide[i].LocationB, 12.f, 12, FColor::Blue, true);
		DrawDebugString(GetWorld(), ResultObjectFaceSide[i].LocationB, "LocB");
	}
}

void UObjectFaceSide::PrintAllDistancePoint()
{
	for (int32 Index = 0; Index < DistanceCloseToCharacter.Num(); Index++)
	{
		DrawDebugSphere(GetWorld(), DistanceCloseToCharacter[Index], 12.f, 12, FColor::Orange, true);
		DrawDebugString(GetWorld(), DistanceCloseToCharacter[Index], FString::SanitizeFloat(ResultShortestDistance[Index]));
	}
}

void UObjectFaceSide::PrintAllDistancePointPercent()
{

	for (int32 Index = 0; Index < DistanceCloseToCharacter.Num(); Index++)
	{
		DrawDebugSphere(GetWorld(), DistanceCloseToCharacter[Index], 12.f, 12, FColor::Orange, true);
		DrawDebugString(GetWorld(), DistanceCloseToCharacter[Index], FString::SanitizeFloat(ResultShortestDistance[Index]));
	}
}

/*
DistanceA = (LastCharacterLocation - LeftSideData[i].LocationA).Size();
		DistanceB = (LastCharacterLocation - ResultObjectFaceSide[i].LocationA).Size();
		DrawDebugString(GetWorld(), LeftSideData[i].LocationA, FString::SanitizeFloat(DistanceA));
		DrawDebugString(GetWorld(), ResultObjectFaceSide[i].LocationA, FString::SanitizeFloat(DistanceB));

		DistanceA = (LastCharacterLocation - RightSideData[i].LocationA).Size();
		DistanceB = (LastCharacterLocation - ResultObjectFaceSide[i].LocationB).Size();
		DrawDebugString(GetWorld(), RightSideData[i].LocationA, FString::SanitizeFloat(DistanceA));
		DrawDebugString(GetWorld(), ResultObjectFaceSide[i].LocationB, FString::SanitizeFloat(DistanceB));

		//DrawDebugSphere(GetWorld(), ResultObjectFaceSide[i].LocationB, 12.f, 12, FColor::Black, true);

		DrawDebugLine(GetWorld(), LeftSideData[i].LocationA, LeftSideData[i].LocationB, FColor::Magenta, true);
		DrawDebugLine(GetWorld(), RightSideData[i].LocationA, RightSideData[i].LocationB, FColor::Magenta, true);
*/

void UObjectFaceSide::ReloadDistance2D()
{
	DistanceCloseToCharacter.Add(LastCharacterLocation);
	int32 Count = 1;
	//need to save first location to begin find the route

	for (int32 i = 0; i < LeftSideData.Num(); i++) //apply face side there
	{
		//float DistanceA, DistanceB;

		DistanceCloseToCharacter.Add(LeftSideData[i].LocationA);
		DistanceCloseToCharacter.Add(ResultObjectFaceSide[i].LocationA);
		DistanceCloseToCharacter.Add(RightSideData[i].LocationA);
		DistanceCloseToCharacter.Add(ResultObjectFaceSide[i].LocationB);

		Count += 4;
	}
	for (int32 i = 0; i < Count; i++)
	{
		for (int32 j = 0; j < Count; j++)
		{
			if (i == j)
			{
				Distance2DArray.Add(0.f);
			}
			else
			{
				Distance2DArray.Add(FMath::Abs((DistanceCloseToCharacter[i] - DistanceCloseToCharacter[j]).Size()));
			}

		}
	}
	Distance2DSize = Count;

	ResultShortestDistance = CheckDistanceByDijkstra();

	ApplyDistance2DToWorld();
	/*FString printResult = "";
	for (int i = 0; i < Count; i++)
	{
		for (int j = 0; j < Count; j++)
		{
			printResult += FString::SanitizeFloat(Distance2DArray[Count * i + j]) + " ";
		}
		UE_LOG(LogTemp, Warning, TEXT("%s "), *printResult);
		printResult = "";
	}*/
}

/**
* We will jump index 0 (character loc) to +2 with
* target location
*/
void UObjectFaceSide::ApplyDistance2DToWorld()
{
	DistanceCloseToCharacter.RemoveAt(0);
	ResultShortestDistance.RemoveAt(0);
	for (int32 i = 0; i < DistanceCloseToCharacter.Num(); i++)
	{
		DistanceCloseToCharacter.RemoveAt(i);
		ResultShortestDistance.RemoveAt(i);
	}
}

TArray<float> UObjectFaceSide::CheckDistanceByDijkstra()
{
	//DistanceCloseToCharacter
	TArray<float>PointResult;
	TArray<bool>PointCheck;

	PointResult.Add(0); 
	PointCheck.Add(false);
	//start point
	for (int i = 0; i < Distance2DSize - 1; i++)
	{
		PointResult.Add(UE_DOUBLE_BIG_NUMBER);
		PointCheck.Add(false);
	}
		
	UE_LOG(LogTemp, Warning, TEXT("Size: %d %d %d"), PointResult.Num(), Distance2DArray.Num(), LeftSideData.Num() + RightSideData.Num());
	for (int32 count = 0; count < Distance2DSize - 1; count++)
	{
		int32 CurrentDistance = MinDistanceDijkstra(PointResult, PointCheck);
		PointCheck[CurrentDistance] = true;
		for (int i = 0; i < Distance2DSize; i++)
		{
			if (PointCheck[i] == false &&
				Distance2DArray[Distance2DSize * CurrentDistance + i] &&
				PointResult[CurrentDistance] != UE_DOUBLE_BIG_NUMBER &&
				PointResult[CurrentDistance] + Distance2DArray[Distance2DSize * CurrentDistance + i] < PointResult[i])
			{
				PointResult[i] = PointResult[CurrentDistance] + Distance2DArray[Distance2DSize * CurrentDistance + i];
			}
		}
	}
	return PointResult;
}

void UObjectFaceSide::PrintShortestValueFromDijkstra(TArray<float>PointResult)
{
	for (int32 Index = 0; Index < Distance2DSize; Index++)
	{
		
	}
}
int32 UObjectFaceSide::MinDistanceDijkstra(TArray<float> PointResult, TArray<bool> PointCheck)
{
	if (PointResult.IsEmpty() || PointCheck.IsEmpty()) return 0;
	float Min = UE_DOUBLE_BIG_NUMBER;
	int32 MinIndex = 0;
	for (int32 i = 0; i < Distance2DSize; i++)
	{
		if (PointCheck[i] == false && PointResult[i] <= Min)
		{
			Min = PointResult[i];
			MinIndex = i;
		}
	}
	return MinIndex;
}



void UObjectFaceSide::Clear()
{
	LeftSideData.Empty();
	RightSideData.Empty();
	ResultObjectFaceSide.Empty();
	DistanceCloseToCharacter.Empty();
	ResultShortestDistance.Empty();
	Distance2DArray.Empty();
	ItemData.Empty();
	Distance2DSize = 0;
}

void UObjectFaceSide::SetupLineObjectSideData(EObjectFaceSideType NameType, FVector ObjectLocation, FVector ObjectExtent, FVector CharacterLocation, FVector ClosestObjectLocation)
{
	//if (NumObjectFaceReceive == MAX_OBJECT_FACE_RECEIVED) return;

	FLineStructDebug ObjectFaceSideData;
	InvalidTwoPointForm = false;

	SetLineObjectSideData(NameType, ObjectLocation, ObjectExtent, ObjectFaceSideData);
	FVector TwoPointFormPoint = TwoPointFormGenerator(CharacterLocation, ClosestObjectLocation, ObjectFaceSideData);
	
	if (!InvalidTwoPointForm && 
		IsInsideFaceSide(
			FVectorConvertRoundDouble(TwoPointFormPoint), 
			FLineStructConvertRoundDouble(ObjectFaceSideData)))
	{
		//NumObjectFaceReceive += 1;
		double CurrentDistance = GetDistance(CharacterLocation, TwoPointFormPoint);
		if (BestObjectFaceSide_Distance > CurrentDistance)
		{
			DrawDebugSphere(GetWorld(), TwoPointFormPoint, 10.f, 12, FColor::Black, true);
			BestObjectFaceSide_Distance = CurrentDistance;
			BestObjectFaceSide = NameType;
			UE_LOG(LogTemp, Warning, TEXT("Found Data: %f"), BestObjectFaceSide_Distance);

			switch (BestObjectFaceSide)
			{
			case EObjectFaceSideType::EOFST_UP: //up
				UE_LOG(LogTemp, Warning, TEXT("Update to up"));
				break;

			case EObjectFaceSideType::EOFST_DOWN: //down
				UE_LOG(LogTemp, Warning, TEXT("Update to down"));
				break;

			case EObjectFaceSideType::EOFST_LEFT: //left
				UE_LOG(LogTemp, Warning, TEXT("Update to left"));
				break;

			case EObjectFaceSideType::EOFST_RIGHT: //right
				UE_LOG(LogTemp, Warning, TEXT("Update to right"));
				break;

			case EObjectFaceSideType::EOFST_MAX:
				UE_LOG(LogTemp, Warning, TEXT("No Update"));
				break;

			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("-----------------------------------------------"));
}

/*
* d: n=(Start.Y - End.Y, End.X - Start.X)
*
* 		AB = (c - a; d - b)
* 		Start(a, b) | End(c, d)
*
* 		FVector Start = FVector(Origin.X - ObjectExtent.X, Origin.Y + ObjectExtent.Y, Origin.Z);
* 		FVector End = FVector(Origin.X + ObjectExtent.X, Origin.Y + ObjectExtent.Y, Origin.Z);
* 		DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, true);
* 		//down
* 		=> (Origin.Y + ObjectExtent.Y) - (Origin.Y + ObjectExtent.Y),
* 		   (Origin.X + ObjectExtent.X) - (Origin.X - ObjectExtent.X),
*
* 		   a = Origin.X - ObjectExtent.X
* 		   b = Origin.Y + ObjectExtent.Y
* 		   c = Origin.X + ObjectExtent.X
* 		   d = Origin.Y + ObjectExtent.Y
*
* 		Start = FVector(Origin.X - ObjectExtent.X, Origin.Y - ObjectExtent.Y, Origin.Z);
* 		End = FVector(Origin.X + ObjectExtent.X, Origin.Y - ObjectExtent.Y, Origin.Z);
* 		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, true);
* 		//up
* 		=> 	(Origin.Y - ObjectExtent.Y) - (Origin.Y - ObjectExtent.Y),
* 			(Origin.X + ObjectExtent.X) - (Origin.X - ObjectExtent.X),
*
* 		   a = Origin.X - ObjectExtent.X
* 		   b = Origin.Y - ObjectExtent.Y
* 		   c = Origin.X + ObjectExtent.X
* 		   d = Origin.Y - ObjectExtent.Y
*
* 		Start = FVector(Origin.X - ObjectExtent.X, Origin.Y - ObjectExtent.Y, Origin.Z);
* 		End = FVector(Origin.X - ObjectExtent.X, Origin.Y + ObjectExtent.Y, Origin.Z);
* 		DrawDebugLine(GetWorld(), Start, End, FColor::Black, true);
* 		//left
* 		=> 	(Origin.Y - ObjectExtent.Y) - (Origin.Y + ObjectExtent.Y),
* 			(Origin.X - ObjectExtent.X) - (Origin.X - ObjectExtent.X),
*
* 		   a = Origin.X - ObjectExtent.X
* 		   b = Origin.Y - ObjectExtent.Y
* 		   c = Origin.X - ObjectExtent.X
* 		   d = Origin.Y + ObjectExtent.Y
*
* 		Start = FVector(Origin.X + ObjectExtent.X, Origin.Y - ObjectExtent.Y, Origin.Z);
* 		End = FVector(Origin.X + ObjectExtent.X, Origin.Y + ObjectExtent.Y, Origin.Z);
* 		DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, true);
* 		//right
* 		=> 	(Origin.Y - ObjectExtent.Y) - (Origin.Y + ObjectExtent.Y),
* 			(Origin.X + ObjectExtent.X) - (Origin.X + ObjectExtent.X),
*
* 		   a = Origin.X + ObjectExtent.X
* 		   b = Origin.Y - ObjectExtent.Y
* 		   c = Origin.X + ObjectExtent.X
* 		   d = Origin.Y + ObjectExtent.Y
*/
void UObjectFaceSide::SetLineObjectSideData(EObjectFaceSideType NameType, FVector Location, FVector Extent, FLineStructDebug& ObjectSideData)
{
	switch (NameType)
	{
	case EObjectFaceSideType::EOFST_DOWN:
		UE_LOG(LogTemp, Warning, TEXT("DOWN"));
		ObjectSideData.LocationA.X = Location.X - Extent.X;
		ObjectSideData.LocationA.Y = Location.Y - Extent.Y;
		ObjectSideData.LocationB.X = Location.X + Extent.X;
		ObjectSideData.LocationB.Y = Location.Y - Extent.Y;

		break;

	case EObjectFaceSideType::EOFST_UP:
		UE_LOG(LogTemp, Warning, TEXT("UP"));
		ObjectSideData.LocationA.X = Location.X - Extent.X;
		ObjectSideData.LocationA.Y = Location.Y + Extent.Y;
		ObjectSideData.LocationB.X = Location.X + Extent.X;
		ObjectSideData.LocationB.Y = Location.Y + Extent.Y;

		break;

	case EObjectFaceSideType::EOFST_LEFT:
		UE_LOG(LogTemp, Warning, TEXT("LEFT"));
		ObjectSideData.LocationA.X = Location.X - Extent.X;
		ObjectSideData.LocationA.Y = Location.Y - Extent.Y;
		ObjectSideData.LocationB.X = Location.X - Extent.X;
		ObjectSideData.LocationB.Y = Location.Y + Extent.Y;

		break;

	case EObjectFaceSideType::EOFST_RIGHT:
		UE_LOG(LogTemp, Warning, TEXT("RIGHT"));
		ObjectSideData.LocationA.X = Location.X + Extent.X;
		ObjectSideData.LocationA.Y = Location.Y - Extent.Y;
		ObjectSideData.LocationB.X = Location.X + Extent.X;
		ObjectSideData.LocationB.Y = Location.Y + Extent.Y;

		break;

	}
	ObjectSideData.LocationA.Z = Location.Z;
	ObjectSideData.LocationB.Z = Location.Z;

}



FVector UObjectFaceSide::LineGenerator(FVector LocationA, FVector LocationB, double ZAxis)
{
	double a = (double) (LocationB.Y - LocationA.Y) / (double) (LocationB.X - LocationA.X);
	UE_LOG(LogTemp, Warning, TEXT("%f - %f"), LocationB.Y, LocationA.Y);
	double b = (double) LocationB.Y - (a * LocationB.X);
	UE_LOG(LogTemp, Warning, TEXT("A = %f, B = %f"), a, b);
	return FVector(a, b, ZAxis);
}

/*
*	y = mx + n or y = ax + b
* 	character: y = (a1)x + b1 => (a1)x - y = -b1 
*	target: y = (a2)x + b2 => (a2)x - y = -b2
*	==> (a1 - a2)x = -b1 + b2 => x = (-b1 + b2) / (a1 - a2)
*/
FVector UObjectFaceSide::TwoPointFormGenerator(FVector CharacterLocation, FVector ClosestObjectLocation, FLineStructDebug& ObjectSideData)
{
	DrawDebugLine(GetWorld(), CharacterLocation, ClosestObjectLocation, FColor::Blue, true);

	bool CharacterSingleLineXAxis = IsSingleLineXAxis(CharacterLocation, ClosestObjectLocation);
	bool ObjectSingleLineXAxis = IsSingleLineXAxis(ObjectSideData.LocationA, ObjectSideData.LocationB);

	UE_LOG(LogTemp, Warning, TEXT("%f - %f"), ClosestObjectLocation.Y, CharacterLocation.Y);

	double x = NULL, y;
	
	FVector CharacterLine, TargetLine;
	FVector TwoPointFormPoint;



	if (CharacterSingleLineXAxis)
	{
		x = ClosestObjectLocation.X;
		UE_LOG(LogTemp, Warning, TEXT("X affect"));
	}
	
	if (ObjectSingleLineXAxis)
	{
		if (x != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Parallel Line"));
			return ClosestObjectLocation; //TODO: check this
		}
		x = ObjectSideData.LocationB.X;
		UE_LOG(LogTemp, Warning, TEXT("x = %f"), x);
		CharacterLine = LineGenerator(CharacterLocation, ClosestObjectLocation, CharacterLocation.Z);
		y = CharacterLine.X * x + CharacterLine.Y;

		UE_LOG(LogTemp, Warning, TEXT("d: y = %fx + %f"), CharacterLine.X, CharacterLine.Y);


		TwoPointFormPoint = FVector(x, y, CharacterLocation.Z);
		UE_LOG(LogTemp, Warning, TEXT("Single Line in Object"));

		return TwoPointFormPoint;
	}

	if (x != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("x = %f"), x);
		TargetLine = LineGenerator(ObjectSideData.LocationA, ObjectSideData.LocationB, CharacterLocation.Z);
		y = TargetLine.X * x + TargetLine.Y;

		UE_LOG(LogTemp, Warning, TEXT("d': y = %fx + %f"), TargetLine.X, TargetLine.Y);

		TwoPointFormPoint = FVector(x, y, CharacterLocation.Z);
		UE_LOG(LogTemp, Warning, TEXT("Single Line in Character"));
		return TwoPointFormPoint;
	}

	CharacterLine = LineGenerator(CharacterLocation, ClosestObjectLocation, CharacterLocation.Z);
	TargetLine =  LineGenerator(ObjectSideData.LocationA, ObjectSideData.LocationB, CharacterLocation.Z);

	if (true)
	{

		UE_LOG(LogTemp, Warning, TEXT("d: y = %fx + %f"), CharacterLine.X, CharacterLine.Y);
		UE_LOG(LogTemp, Warning, TEXT("d': y = %fx + %f"), TargetLine.X, TargetLine.Y);
	
		//DrawDebugLine(GetWorld(), CharacterLocation, CharacterLine, FColor::Blue, true);
		//DrawDebugLine(GetWorld(), ClosestObjectLocation, CharacterLine, FColor::Blue, true);
		//DrawDebugLine(GetWorld(), ObjectSideData.LocationA, TargetLine, FColor::White, true);
		//DrawDebugLine(GetWorld(), ObjectSideData.LocationB, TargetLine, FColor::White, true);
	}
	

	if (CharacterLine.X == TargetLine.X)
	{
		if (CharacterLine.Y != TargetLine.Y)
		{
			UE_LOG(LogTemp, Warning, TEXT("Seperate Line"));
			InvalidTwoPointForm = true;
			return FVector();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Inf Line"));
			TwoPointFormPoint = FVector(FMath::RoundFromZero(ClosestObjectLocation.X), FMath::RoundFromZero(ClosestObjectLocation.Y), CharacterLocation.Z);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Normal Line"));
		x = (double) (-CharacterLine.Y + (TargetLine.Y)) / (CharacterLine.X - TargetLine.X);
		y = (double) CharacterLine.X * x + CharacterLine.Y;

		TwoPointFormPoint = FVector(x, y, CharacterLocation.Z);
	}

	UE_LOG(LogTemp, Warning, TEXT("New Point N(%f, %f)"), TwoPointFormPoint.X, TwoPointFormPoint.Y);
	
	return TwoPointFormPoint;

}

FVector UObjectFaceSide::FVectorConvertRoundDouble(FVector GivenVector)
{
	return FVector(
		FMath::RoundToDouble(GivenVector.X),
		FMath::RoundToDouble(GivenVector.Y),
		FMath::RoundToDouble(GivenVector.Z)
		);
}

FLineStructDebug UObjectFaceSide::FLineStructConvertRoundDouble(FLineStructDebug GivenLine)
{
	return FLineStructDebug{
		FVectorConvertRoundDouble(GivenLine.LocationA),
		FVectorConvertRoundDouble(GivenLine.LocationB)
	};
}

EObjectFaceSideType UObjectFaceSide::GetCollisionObjectSide(FVector ObjectLocation, FVector ObjectExtent, FVector CharacterLocation, FVector ClosestObjectLocation)
{
	NumObjectFaceReceive = 0;
	BestObjectFaceSide_Distance = UE_DOUBLE_BIG_NUMBER;

	if (true)
	{
		switch (EObjectFaceSideType FaceSideType = EObjectFaceSideType::EOFST_UP)
		{
		case EObjectFaceSideType::EOFST_UP: //up

			SetupLineObjectSideData(FaceSideType, ObjectLocation, ObjectExtent, CharacterLocation, ClosestObjectLocation);
			FaceSideType = EObjectFaceSideType::EOFST_DOWN;

		case EObjectFaceSideType::EOFST_DOWN: //down

			SetupLineObjectSideData(FaceSideType, ObjectLocation, ObjectExtent, CharacterLocation, ClosestObjectLocation);
			FaceSideType = EObjectFaceSideType::EOFST_LEFT;

		case EObjectFaceSideType::EOFST_LEFT: //left

			SetupLineObjectSideData(FaceSideType, ObjectLocation, ObjectExtent, CharacterLocation, ClosestObjectLocation);
			FaceSideType = EObjectFaceSideType::EOFST_RIGHT;

		case EObjectFaceSideType::EOFST_RIGHT: //right
			SetupLineObjectSideData(FaceSideType, ObjectLocation, ObjectExtent, CharacterLocation, ClosestObjectLocation);
			FaceSideType = EObjectFaceSideType::EOFST_MAX;

		case EObjectFaceSideType::EOFST_MAX:
			break;
		}
	}
	return BestObjectFaceSide;
}

double UObjectFaceSide::GetDistance(FVector GivenA, FVector GivenB)
{
	return FMath::Square(
		FMath::Pow(GivenA.X - GivenB.X, 2) +
		FMath::Pow(GivenA.Y - GivenB.Y, 2));
}

bool UObjectFaceSide::IsInsideFaceSide(FVector TwoPointFormPoint, FLineStructDebug ObjectSideData)
{
	bool IsInRangeX = (ObjectSideData.LocationA.X <= TwoPointFormPoint.X && 
		ObjectSideData.LocationB.X >= TwoPointFormPoint.X)
		|| (ObjectSideData.LocationA.X >= TwoPointFormPoint.X && 
			ObjectSideData.LocationB.X <= TwoPointFormPoint.X);
	bool IsInRangeY = (ObjectSideData.LocationA.Y <= TwoPointFormPoint.Y && 
		ObjectSideData.LocationB.Y >= TwoPointFormPoint.Y)
		|| (ObjectSideData.LocationA.Y >= TwoPointFormPoint.Y && 
			ObjectSideData.LocationB.Y <= TwoPointFormPoint.Y);

	UE_LOG(LogTemp, Warning, TEXT("%d | %d"),
		IsInRangeX,
		IsInRangeY
	);

	return IsInRangeX && IsInRangeY;
}

bool UObjectFaceSide::IsSingleLineXAxis(FVector A, FVector B)
{
	UE_LOG(LogTemp, Warning, TEXT("Loc: %f | %f | %f | %f"), A.X, A.Y, B.X, B.Y);
	return (A.X == B.X && A.Y != B.Y);
}

bool UObjectFaceSide::IsObjectInRange(FRotator CharacterRotation, FRotator ObjectRotation)
{
	float YawFromObject = CharacterRotation.Yaw - ObjectRotation.Yaw;
	UE_LOG(LogTemp, Warning, TEXT("Yaw Scan: %f"), YawFromObject);
	return 135.f <= YawFromObject && YawFromObject <= 225.f;
}

