#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_AmmoSwitch UMETA(DisplayName = "Ammo Switch"),
	ECS_HitReact UMETA(DisplayName = "Hit React"),
	ECS_Attack UMETA(DisplayNmae = "Attack"),
	ECS_Slide UMETA(DisplayName = "Slide"),
	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};