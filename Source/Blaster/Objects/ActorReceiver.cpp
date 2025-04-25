#include "ActorReceiver.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "Engine/ObjectLibrary.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/Path/CalculatePath.h"

// Sets default values
UActorReceiver::UActorReceiver()
{

}

void UActorReceiver::SetupActorReceiver()
{
	MaxScanRange = DEFAULT_ACTOR_RECEIVER_RANGE;
	LimitObjectsResult = 100;

	SetupDefaultObject();
}

void UActorReceiver::SetupDefaultObject()
{
	RestrictObject.Empty();

	RestrictObject.Add("LevelSequenceActor");
	RestrictObject.Add("Floor");
	RestrictObject.Add("RecastNavMesh");
	RestrictObject.Add("SkyAtmosphere");
	RestrictObject.Add("BP_IdlePath");
	RestrictObject.Add("NavMeshBoundsVolume");

}


void UActorReceiver::UpdateActorReceiver(EScanObjectType GivenObjectType, FVector GivenCharacterLocation)
{
	//ObjectsResult.Empty();
	ResetObjectsResult();
	IncludeObject.Empty();

	switch (GivenObjectType)
	{
	case EScanObjectType::ESOT_Character:
		SetCharacterOnly();
		break;
	case EScanObjectType::ESOT_Object:
		SetObjectOnly();
		break;
	case EScanObjectType::ESOT_Weapon:
		SetWeaponOnly();
		break;
	case EScanObjectType::ESOT_Pickups:
		SetPickupOnly();
		break;
	case EScanObjectType::ESOT_AllObjects:
		SetAll();
		break;
	}

	CheckAvailableObjects(GivenCharacterLocation);
}

void UActorReceiver::SetCharacterOnly()
{
	IncludeObject.Add("BP_CarbinePearls");
	IncludeObject.Add("BP_BlasterCharacter");
	//

}

void UActorReceiver::SetObjectOnly()
{
	IncludeObject.Add("StaticMeshActor");
}

void UActorReceiver::SetWeaponOnly() //will change into loadblueprint later
{
	TArray<FAssetData> AssetDatas;
	TArray<FAssetData> AllAssetDatas;

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(AActor::StaticClass(), true, GIsEditor);
	ObjectLibrary->AddToRoot();

	for (EWeaponType Val : TEnumRange<EWeaponType>())
	{
		FString WeaponBPName = UEnum::GetDisplayValueAsText(Val).ToString();
		ObjectLibrary->LoadBlueprintAssetDataFromPath(TEXT("/Game/Blueprints/Weapon/" + WeaponBPName));

		ObjectLibrary->GetAssetDataList(AssetDatas);

		for (FAssetData Item : AssetDatas)
		{
			IncludeObject.Add(Item.AssetName.ToString());
		}
	}
}

void UActorReceiver::SetPickupOnly()
{

}

void UActorReceiver::SetAll()
{

}

void UActorReceiver::CheckAvailableObjects(FVector GivenCharacterLocation)
{
	int32 CurrentObjCount = 0;

	AActor* GivenActor = NewObject<AActor>();
	GivenActor->SetActorLocation(GivenCharacterLocation);

	for (TObjectIterator<AActor> Itr; Itr && CurrentObjCount <= LimitObjectsResult; ++Itr)
	{
		float Distance = GivenActor->GetDistanceTo(*Itr);

		FVector Location;
		float ShortestDistance = Itr->ActorGetDistanceToCollision(GivenCharacterLocation, ECollisionChannel::ECC_Visibility, Location);
		
		if (Distance > 0 && Distance <= MaxScanRange
			|| ShortestDistance > 0 && ShortestDistance <= MaxScanRange)
		{
			bool IsFound = false;
			bool IsObjectRequirement = IncludeObject.Num() > 0;
			if (IsObjectRequirement)
			{
				for (FString NameObject : IncludeObject)
				{
					if ((*Itr)->GetName().Contains(NameObject))
					{
						CurrentObjCount++;
						ObjectsResult.Add(*Itr);
						IsFound = true;
						break;
					}
				}
			}
			else
			{
				CurrentObjCount++;
				ObjectsResult.Add(*Itr);
				IsFound = true;
			}

			if (IsFound) //debug
			{
				DrawDebugLine(GetWorld(),
					GivenCharacterLocation,
					Distance <= MaxScanRange ? Itr->GetActorLocation() : Location,
					FColor::White,
					true);

				UE_LOG(LogTemp, Warning, TEXT("%s: %f"), *Itr->GetName(), ShortestDistance);
			}
			
		}
	}
	//DrawDebugSphere(GetWorld(), GivenCharacterLocation, MaxScanRange, 10, FColor::Red, true);
}

void UActorReceiver::SetScanRange(float GivenRange)
{
	MaxScanRange = GivenRange;
}

void UActorReceiver::SetLimitObjectsResult(int32 NoOfObjects)
{
	LimitObjectsResult = NoOfObjects;
	UE_LOG(LogTemp, Warning, TEXT("Limit: %d %d"), LimitObjectsResult, NoOfObjects);
}

void UActorReceiver::ResetObjectsResult()
{
	ObjectsResult.Empty(LimitObjectsResult);
}

TArray<AActor*> UActorReceiver::GetObjectsResultData()
{
	return ObjectsResult;
}
