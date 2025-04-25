#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),
	EWT_RocketLauncher UMETA(DisplayName = "RocketLauncher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "SubMachineGun"),
	EWT_Shotgun UMETA(DisplayName = "ShotGun"),
	EWT_GrenadeLauncher UMETA(DisplayName = "GrenadeLauncher"),
	EWT_SniperRifle UMETA(DisplayName = "SniperRifle"),
	EWT_MeleeWeapon UMETA(DisplayName = "MeleeWeapon"),

	EWT_MAX UMETA(DisplayName = "DefaultMax"),

};
ENUM_RANGE_BY_COUNT(EWeaponType, EWeaponType::EWT_MAX);