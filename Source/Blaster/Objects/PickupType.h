#pragma once

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	EPT_AmmoPickups UMETA(DisplayName = "AmmoPickups"),
	EPT_BuffPickups UMETA(DisplayName = "BuffPickups"),

	EPT_MAX UMETA(DisplayName = "DefaultMax"),

};
ENUM_RANGE_BY_COUNT(EPickupType, EPickupType::EPT_MAX);