#pragma once

UENUM(BlueprintType)
enum class ERangePathType : uint8
{
	ERPT_NoInRange UMETA(DisplayName = "NoInRange"),
	ERPT_InRangeX UMETA(DisplayName = "InRange_X"),
	ERPT_InRangeY UMETA(DisplayName = "InRange_Y"),
	ERPT_InRangeXY UMETA(DisplayName = "InRange_XY")

};