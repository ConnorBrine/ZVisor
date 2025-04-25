#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "SpawnInitializeComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API USpawnInitializeComponent : public UDataAsset
{
	GENERATED_BODY()
public:


	TArray<UTexture2D*> LoadWeaponHUD();

	//As Object cannot be use in this ScanObjectType
	TArray<AActor*> LoadActor(EScanObjectType SpawnType);
	AActor* LoadNotification();

	FDataTableRowHandle LoadPathChapterHandle();
	

private:
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<UTexture2D>> WeaponHUD;

	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<AActor>> Character;

	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<AActor>> Weapon;

	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<AActor>> WeaponAmmoAttach;

	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<AActor>> WeaponMagAttach;

	//with attach hand, this will be added more attach later

	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<AActor>> PickupAmmo;

	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<AActor>> PickupHealth;

	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<AActor>> Trigger;
	
	UPROPERTY(EditAnywhere)
	TArray<TSoftClassPtr<AActor>> PathChapterItem;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> PathChapter;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AActor> Notification; // can be more, but use 1 icon this time 
};
