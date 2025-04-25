#pragma once

UENUM(BlueprintType)
enum class EHitboxType : uint8
{
	EHT_Head UMETA(DisplayName = "Head"),
	EHT_Neck UMETA(DisplayName = "Neck"),
	EHT_Body UMETA(DisplayName = "Body"),
	EHT_Hand UMETA(DisplayName = "Hand"),
	EHT_HandLeft UMETA(DisplayName = "HandLeft"),
	EHT_HandRight UMETA(DisplayName = "HandRight"),
	EHT_Leg UMETA(DisplayName = "Leg"),

	EHT_MAX UMETA(DisplayName = "DefaultMax"),

};