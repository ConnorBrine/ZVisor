#pragma once

UENUM(BlueprintType)
enum class EClearCondition : uint8
{
	ECC_Default UMETA(DisplayName = "Default"),
	ECC_Slay UMETA(DisplayName = "Slay"),
	ECC_Collect UMETA(DisplayName = "Collect"),
	ECC_Travel UMETA(DisplayName = "Travel"),
	ECC_KillCollect UMETA(DisplayName = "Kill Collect"),
	ECC_Capture UMETA(DisplayName = "Capture")
};