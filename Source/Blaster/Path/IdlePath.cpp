#include "IdlePath.h"

AIdlePath::AIdlePath()
{ 	
	PrimaryActorTick.bCanEverTick = false;

	CalculatePath = NewObject<UCalculatePath>();
}

FVector AIdlePath::GetIdlePoint(int32 const Index) const
{
	return IdlePoints[Index];
}

int32 AIdlePath::Num() const
{
	return IdlePoints.Num();
}

FVector AIdlePath::ShortestDistance(FVector GivenLocation, FGeneratePath PathStats) 
{
	PathStats.ShortestDistanceIndex = ShortestDistancePathIndex(GivenLocation, PathStats);
	FVector ShortestDistancePathPoint = GetIdlePoint(PathStats.ShortestDistanceIndex);

	return GeneratePathPoint(GivenLocation, ShortestDistancePathPoint, PathStats);

	//TODO: struct cannot return exact number so this function doesnt make sense
}

FVector AIdlePath::ShortestDistanceByVertices(FVector GivenLocation, FGeneratePath PathStats) const
{
	FVector VectexLocationMin = FVector(PathStats.MinX - GetActorLocation().X, PathStats.MinY - GetActorLocation().Y, GetActorLocation().Z);
	FVector VectexLocationMin2 = FVector(PathStats.MinX - GetActorLocation().X, PathStats.MaxY - GetActorLocation().Y, GetActorLocation().Z);
	FVector VectexLocationMin3 = FVector(PathStats.MaxX - GetActorLocation().X, PathStats.MinY - GetActorLocation().Y, GetActorLocation().Z);
	FVector VectexLocationMin4 = FVector(PathStats.MaxX - GetActorLocation().X, PathStats.MaxY - GetActorLocation().Y, GetActorLocation().Z);

	VectexLocationMin = (GetDistance(GivenLocation, VectexLocationMin) < GetDistance(GivenLocation, VectexLocationMin3)) ?
		VectexLocationMin : VectexLocationMin3;
	VectexLocationMin2 = (GetDistance(GivenLocation, VectexLocationMin2) < GetDistance(GivenLocation, VectexLocationMin4)) ?
		VectexLocationMin2 : VectexLocationMin4;

	return (GetDistance(GivenLocation, VectexLocationMin) < GetDistance(GivenLocation, VectexLocationMin2)) ?
		VectexLocationMin : VectexLocationMin2;

	//TODO: Later this function will optimize to run better
}

int32 AIdlePath::ShortestDistancePathIndex(FVector GivenLocation, FGeneratePath PathStats) const
{
	int32 MaxIndex = Num() - 1;

	int32 ShortestDistanceIndex = 0;
	double ShortestDistance = GetDistance(GivenLocation, GetIdlePoint(0));

	UE_LOG(LogTemp, Warning, TEXT("First Point[0]: %f | %f = %f"), GetIdlePoint(0).X, GetIdlePoint(0).Y, ShortestDistance);

	for (int32 Index = 0; Index <= MaxIndex; ++Index)
	{
		FVector GivenPoint = GetIdlePoint(Index);
		double GivenDistance = GetDistance(GivenLocation, GivenPoint);

		UE_LOG(LogTemp, Warning, TEXT("Point[%d]: %f | %f = %f"), Index, GivenPoint.X, GivenPoint.Y, GivenDistance);

		if (GivenDistance < ShortestDistance)
		{
			ShortestDistance = GivenDistance;
			ShortestDistanceIndex = Index;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Change to Point[%d]: %f"), ShortestDistanceIndex, ShortestDistance);
	}
	return ShortestDistanceIndex;
}

int32 AIdlePath::NextIdlePathIndex(FVector GivenLocation, FGeneratePath PathStats) const
{
	int32 NoOfPoints = Num();
	int32 ShortestDistanceIndex = PathStats.ShortestDistanceIndex;
	
	int32 NextIndex = (ShortestDistanceIndex + 1) % NoOfPoints;

	switch (PathStats.CurrentLocationRange)
	{
	case ERangePathType::ERPT_InRangeX:
		UE_LOG(LogTemp, Warning, TEXT("Range X"));
		if (IsSameDirection(GivenLocation.X, GetIdlePoint(ShortestDistanceIndex).X, GetIdlePoint(NextIndex).X))
		{
			return NextIndex;
		}
		else
		{
			return ShortestDistanceIndex;
		}
		

	case ERangePathType::ERPT_InRangeY:
		UE_LOG(LogTemp, Warning, TEXT("Range Y"));
		if (IsSameDirection(GivenLocation.Y, GetIdlePoint(ShortestDistanceIndex).Y, GetIdlePoint(NextIndex).Y))
		{
			return NextIndex;
		}
		else
		{
			return ShortestDistanceIndex;
		}
		

	case ERangePathType::ERPT_InRangeXY:
		UE_LOG(LogTemp, Warning, TEXT("Range XY")); //Random Move inside XY
		NextIndex = ShortestDistancePathIndex(GivenLocation, PathStats);
		UE_LOG(LogTemp, Warning, TEXT("Character next index: %d"), NextIndex);
		return NextIndex;

	case ERangePathType::ERPT_NoInRange:
		return ShortestDistanceIndex;
	}
	return 0;
}

bool AIdlePath::IsValidIdlePathRange(FVector GivenPoint, FGeneratePath PathVertices) const
{
	bool InRange_X = GivenPoint.X <= PathVertices.MaxX - GetActorLocation().X && GivenPoint.X >= PathVertices.MinX - GetActorLocation().X;
	bool InRange_Y = GivenPoint.Y <= PathVertices.MaxY - GetActorLocation().Y && GivenPoint.Y >= PathVertices.MinY - GetActorLocation().Y;

	bool Inside_X = GivenPoint.X == PathVertices.MaxX - GetActorLocation().X && GivenPoint.X == PathVertices.MinX - GetActorLocation().X;
	bool Inside_Y = GivenPoint.Y == PathVertices.MaxY - GetActorLocation().Y && GivenPoint.Y == PathVertices.MinY - GetActorLocation().Y;
	
	return (InRange_X && Inside_Y) || (Inside_X && InRange_Y) || (InRange_X && InRange_Y);
	//TODO: GetActorLocation cannot be make sense with this function, and somehow this coding function make thing kinda bad imo
}

FVector AIdlePath::GeneratePathPoint(FVector VectorA, FVector VectorB, FGeneratePath PathStats) const
{
	FVector Point1 = FVector(VectorA.X, VectorB.Y, VectorA.Z);
	FVector Point2 = FVector(VectorB.X, VectorA.Y, VectorA.Z);

	if (IsValidIdlePathRange(Point1, PathStats))
	{
		if (IsValidIdlePathRange(Point2, PathStats))
		{
			return GetDistance(VectorA, Point1) < GetDistance(VectorA, Point2) ? Point1 : Point2;
		}
		return Point1;
	}
	return Point2;
}

double AIdlePath::GetDistance(FVector GivenA, FVector GivenB) const
{
	return FMath::Square(
		FMath::Pow(GivenA.X - GivenB.X, 2) +
		FMath::Pow(GivenA.Y - GivenB.Y, 2));
} //migrate

FGeneratePath AIdlePath::AssignRangePath(double ValueMin, double ValueMax, FGeneratePath PathStats) const
{
	PathStats.MinForwardRange = ValueMin;
	PathStats.MaxForwardRange = ValueMax;
	return PathStats;
}

bool AIdlePath::IsSameDirection(double GivenPoint, double Point, double NextPoint) const
{
	UE_LOG(LogTemp, Error, TEXT("Single - Given to Point: %f | Next Index to Point: %f [%f]"), GivenPoint, Point,  NextPoint);
	UE_LOG(LogTemp, Error, TEXT("Apply - Given to Point: %f | Next Index to Point: %f "), (GivenPoint - Point), (NextPoint - Point));
	
	UE_LOG(LogTemp, Error, TEXT("Same Direction = %s"), (GivenPoint - Point) == 0
		|| (NextPoint - Point) == 0
		|| (GivenPoint - Point) > 0 && (NextPoint - Point) > 0
		|| (GivenPoint - Point) < 0 && (NextPoint - Point) < 0 ? TEXT("TRUE)") : TEXT("FALSE"));

	return (GivenPoint - Point) == 0 
		|| (NextPoint - Point) == 0
		|| (GivenPoint - Point) > 0 && (NextPoint - Point) > 0
		|| (GivenPoint - Point) < 0 && (NextPoint - Point) < 0;
} //migrate







