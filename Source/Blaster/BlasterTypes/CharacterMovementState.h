#pragma once

UENUM(BlueprintType)
enum class ECharacterMovementState : uint8
{
	ECMS_Run UMETA(DisplayName = "Run"),
	ECMS_Slide UMETA(DisplayName = "Slide"),
	ECMS_Walk UMETA(DisplayName = "Walk"),
	ECMS_Aim UMETA(DisplayName = "Aim")
};