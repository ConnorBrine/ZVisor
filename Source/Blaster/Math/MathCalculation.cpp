#include "MathCalculation.h"

double UMathCalculation::GetDistance(FVector GivenA, FVector GivenB)
{
	return FMath::Sqrt(
		FMath::Pow(GivenA.X - GivenB.X, 2) +
		FMath::Pow(GivenA.Y - GivenB.Y, 2) +
		FMath::Pow(GivenA.Z - GivenB.Z, 2)
	);
}
