#include "CalculatePath.h"
//#include "Engine/Classes/Components/PrimitiveComponent.h"

FVector UCalculatePath::GetClosestObjectPoint(FVector GivenLocation, AActor* Object) const
{
	//FVector ObjectRootPoint = Object->GetActorLocation();
	//FVector Point1 = FVector(GivenLocation.X, ObjectRootPoint.Y, GivenLocation.Z);
	//FVector Point2 = FVector(ObjectRootPoint.X, GivenLocation.Y, GivenLocation.Z);

	//
	//UActorComponent* ActorComponent = Object->GetComponentByClass(UPrimitiveComponent::StaticClass());
	//UPrimitiveComponent* ActorPrimitive = Cast<UPrimitiveComponent>(ActorComponent);

	return FVector();
}

FVector UCalculatePath::GetFurthestObjectPoint(FVector GivenLocation, AActor* Object) const
{
	FVector FurthestPointOnCollision = GivenLocation;
	//float FurthestPointDistanceSqr = -1.f;
	//TInlineComponentArray<UPrimitiveComponent*> Components;
	//Object->GetComponents(Components);
	//UE_LOG(LogTemp, Warning, TEXT("%s Primitive NUM: %d"), *Object->GetName(), Components.Num());
	//for (int32 ComponentIndex = 0; ComponentIndex < Components.Num(); ComponentIndex++)
	//{
	//	UPrimitiveComponent* Primitive = Components[ComponentIndex];

	//	if (Primitive->IsRegistered() && Primitive->IsCollisionEnabled()
	//		&& (Primitive->GetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility) == ECollisionResponse::ECR_Block)) 
	//	{
	//		FVector FurthestPoint;
	//		float DistanceSqr = -1.f;

	//		bool IsRightCollision = Primitive->GetSquaredDistanceToCollision(GivenLocation, DistanceSqr, FurthestPoint);
	//		UE_LOG(LogTemp, Warning, TEXT("Collison Square: %f %f %f"),
	//			FurthestPoint.X, FurthestPoint.Y, FurthestPoint.Z
	//		)
	//		if (!IsRightCollision)
	//		{
	//			
	//			continue;
	//		}
	//		
	//		if((FurthestPointDistanceSqr < 0.f) || (DistanceSqr > FurthestPointDistanceSqr))
	//		{
	//			
	//			//
	//			FurthestPointDistanceSqr = DistanceSqr;
	//			FurthestPointOnCollision = FurthestPoint;
	//			if (DistanceSqr >= UE_BIG_NUMBER)
	//			{
	//				break;
	//			}
	//		}
	//	}
	//}
			//return (FurthestPointDistanceSqr > 0.f ? FMath::Sqrt(FurthestPointDistanceSqr) : FurthestPointDistanceSqr);
	return FurthestPointOnCollision;

}

double UCalculatePath::GetDistance(FVector GivenA, FVector GivenB) const
{
	return FMath::Square(
		FMath::Pow(GivenA.X - GivenB.X, 2) +
		FMath::Pow(GivenA.Y - GivenB.Y, 2));
}

bool UCalculatePath::IsSameDirection(double GivenPoint, double Point, double NextPoint) const
{
	UE_LOG(LogTemp, Error, TEXT("Given to Point: %f | Next Index to Point: %f "), (GivenPoint - Point), (NextPoint - Point));
	UE_LOG(LogTemp, Error, TEXT("Same Direction = %s"), (GivenPoint - Point) == 0
		|| (NextPoint - Point) == 0
		|| (GivenPoint - Point) > 0 && (NextPoint - Point) > 0
		|| (GivenPoint - Point) < 0 && (NextPoint - Point) < 0 ? TEXT("TRUE)") : TEXT("FALSE"));

	return (GivenPoint - Point) == 0
		|| (NextPoint - Point) == 0
		|| (GivenPoint - Point) > 0 && (NextPoint - Point) > 0
		|| (GivenPoint - Point) < 0 && (NextPoint - Point) < 0;
} //Idle Path,...

bool UCalculatePath::IsValidObjectRange(FVector GivenPoint, AActor* Object) const
{

	return true;

}