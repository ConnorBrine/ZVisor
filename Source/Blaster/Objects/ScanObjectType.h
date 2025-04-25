#pragma once

UENUM(BlueprintType)
enum class EScanObjectType : uint8
{
	//"General" item for spawning
	ESOT_Character UMETA(DisplayName = "Character"),
	ESOT_Object UMETA(DisplayName = "Object"),
	ESOT_Weapon UMETA(DisplayName = "Weapon"),
	ESOT_Pickups UMETA(DisplayName = "Pickups"),
	ESOP_Ammo UMETA(DisplayName = "Ammo"),
	ESOP_Health UMETA(DisplayName = "Health"),

	//do not choose Magazine as not correctly what enum, 
	// or we can using it as component of weapon later
	ESOP_Magazine UMETA(DisplayName = Magazine),

	
	//below using with BeginPlay, Function for Attach or Initialize
	//use for spawn item will not make sense
	ESOP_AmmoAttach UMETA(DisplayName = AmmoAttach),
	ESOP_MagAttach UMETA(DisplayName = MagAttach),
	ESOP_Trigger UMETA(DisplayName = Trigger), 
	ESOP_PathChapterItem UMETA(DisplayName = "PathChapterItem"),

	//AllObject like using all "General" item only
	ESOT_AllObjects UMETA(DisplayName = "AllObjects"),

	//Display HUD only, do not use too much, example for spawning
	ESOT_WeaponHUD UMETA(DisplayName = "WeaponHUD")

	//Notification Icon 3D


};