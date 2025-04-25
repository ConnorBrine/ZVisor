#pragma once

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	ECT_Herlobcho UMETA(DisplayName = "Herlobcho"),
	ECT_UnknownCartel UMETA(DisplayName = "UnknownCartel"),

	ECT_MAX UMETA(DisplayName = "DefaultMax"),

};
ENUM_RANGE_BY_COUNT(ECharacterType, ECharacterType::ECT_MAX);