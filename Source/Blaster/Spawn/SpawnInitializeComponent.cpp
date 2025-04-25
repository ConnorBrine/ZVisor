#include "SpawnInitializeComponent.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "Blaster/PathChapter/PathChapter.h"

TArray<UTexture2D*> USpawnInitializeComponent::LoadWeaponHUD()
{
	TArray<UTexture2D*> Target;
	for (TSoftObjectPtr<UTexture2D> ItemPtr : WeaponHUD)
	{
		UTexture2D* Item = ItemPtr.LoadSynchronous();
		if (Item)
		{
			Target.Add(Item);
		}
	}
	return Target;
}

TArray<AActor*> USpawnInitializeComponent::LoadActor(EScanObjectType SpawnType)
{
	TArray<AActor*> Target;
	TArray<TSoftClassPtr<AActor>> ObjectType;
	switch (SpawnType)
	{
	case EScanObjectType::ESOT_Character: ObjectType = Character; break;
	case EScanObjectType::ESOT_Weapon: ObjectType = Weapon; break;
	case EScanObjectType::ESOP_AmmoAttach: ObjectType = WeaponAmmoAttach; break;
	case EScanObjectType::ESOP_MagAttach: ObjectType = WeaponMagAttach; break;
	case EScanObjectType::ESOP_Ammo: ObjectType = PickupAmmo; break;
	case EScanObjectType::ESOP_Health: ObjectType = PickupHealth; break;
	case EScanObjectType::ESOP_Trigger: ObjectType = Trigger; break;
	case EScanObjectType::ESOP_PathChapterItem: ObjectType = PathChapterItem; break;
	}
	for (TSoftClassPtr<AActor> ItemPtr : ObjectType)
	{
		UClass* Class = ItemPtr.LoadSynchronous();
		AActor* Item = Class->GetDefaultObject<AActor>();
		if (Item)
		{
			Target.Add(Item);
		}
	}
	return Target;
}

AActor* USpawnInitializeComponent::LoadNotification()
{
	UClass* Class = Notification.LoadSynchronous();
	return Class->GetDefaultObject<AActor>();
}

FDataTableRowHandle USpawnInitializeComponent::LoadPathChapterHandle()
{
	FDataTableRowHandle PathChapterHandle;
	UDataTable* DataTable;

	PathChapter.LoadSynchronous();
	if (PathChapter.IsValid())
	{
		DataTable = PathChapter.Get();
		PathChapterHandle.DataTable = DataTable;
	}
	return PathChapterHandle;
}


