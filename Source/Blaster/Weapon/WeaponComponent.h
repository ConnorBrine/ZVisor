#pragma once

UENUM(BlueprintType)
enum class EWeaponComponent : uint8
{
	EWC_Magazine UMETA(DisplayName = "Magazine"),
	EWC_Scope UMETA(DisplayName = "Scope"),
	EWC_Barrel UMETA(DisplayName = "Barrel"),
	EWC_Stock UMETA(DisplayName = "Stock"),
	EWC_EjectionPort UMETA(DisplayName = "EjectionPort"),

	EWC_MAX UMETA(DisplayName = "DefaultMax"),

};
