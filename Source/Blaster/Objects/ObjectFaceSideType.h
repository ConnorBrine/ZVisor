#pragma once

UENUM(BlueprintType)
enum class EObjectFaceSideType : uint8
{
	EOFST_UP UMETA(DisplayName = "Up"),
	EOFST_DOWN UMETA(DisplayName = "Down"),
	EOFST_LEFT UMETA(DisplayName = "Left"),
	EOFST_RIGHT UMETA(DisplayName = "Right"),
	EOFST_MAX UMETA(DisplayName = "Max")

};
ENUM_RANGE_BY_COUNT(EObjectFaceSideType, EObjectFaceSideType::EOFST_MAX);