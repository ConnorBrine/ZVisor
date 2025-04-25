#include "SpawnController.h"

#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"

#include "Engine/ObjectLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/AssetManager.h"

#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/PathChapter/PathChapter.h"
#include "Blaster/Objects/Notification.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/Objects/PickupType.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "Blaster/Character/CharacterType.h"
#include "Blaster/Objects/HandAttachObject.h"
#include "Blaster/Spawn/SpawnInitializeComponent.h"

USpawnController::USpawnController()
{

}


TArray<UTexture2D*> USpawnController::LoadWeaponImage()
{
	TArray<UTexture2D*> WeaponData;

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UTexture2D::StaticClass(), false, GIsEditor);
	ObjectLibrary->AddToRoot();
	ObjectLibrary->LoadAssetsFromPath(TEXT("/Game/Assets/Images/Weapon_HUD"));

	ObjectLibrary->GetObjects(WeaponData);

	/*GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString::FromInt(ObjectLibrary->GetObjectCount()));
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString::FromInt(WeaponData.Num()));*/

	//for (UTexture2D* Data : WeaponData)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString(Data->GetName()));
	//}

	return WeaponData;
}

UTexture2D* USpawnController::LoadSingleWeaponImage()
{
	//TArray<UTexture2D*> WeaponData;

	//UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UTexture2D::StaticClass(), false, GIsEditor);
	//ObjectLibrary->AddToRoot();
	//ObjectLibrary->LoadAssetsFromPath(TEXT("/Game/Assets/Images/Weapon_HUD"));

	//ObjectLibrary->GetObjects(WeaponData);
	return nullptr;
}


FString USpawnController::LoadNotification()
{
	TArray<FAssetData> NotificationData;
	FString Path = TEXT("/Game/Blueprints/Notification");
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(ANotification::StaticClass(), true, GIsEditor);
	ObjectLibrary->AddToRoot();
	ObjectLibrary->LoadBlueprintAssetDataFromPath(Path);
	ObjectLibrary->GetAssetDataList(NotificationData);
	UE_LOG(LogTemp, Warning, TEXT("Trigger Data size: %d"), NotificationData.Num());



	if (NotificationData.IsEmpty() == false)
	{
		return (Path + "/" + NotificationData[0].AssetName.ToString());
	}

	return "";
}




FDataTableRowHandle USpawnController::GetEditorPathChapterDataTable()
{
	FDataTableRowHandle PathChapterHandle;

	FSoftObjectPath PathChapterPath =
		FSoftObjectPath(TEXT("/Game/Blueprints/PathChapter/PathChapterDetail.PathChapterDetail"));
	UDataTable* DataTable = Cast<UDataTable>(PathChapterPath.ResolveObject());

	if (DataTable)
	{
		PathChapterHandle.DataTable = DataTable;

		//FString result = PathChapterHandle.DataTable->GetTableAsString(EDataTableExportFlags::UseJsonObjectsForStructs);
		//TArray<FName> NameData = PathChapterHandle.DataTable->GetRowNames();
		//debug
	}
	else
	{
		DataTable = Cast<UDataTable>(PathChapterPath.TryLoad());
		if (DataTable) PathChapterHandle.DataTable = DataTable;

	}
	while (PathChapterHandle.DataTable == nullptr)
	{
		DataTable = Cast<UDataTable>(PathChapterPath.TryLoad());
		if (DataTable) PathChapterHandle.DataTable = DataTable;
	}
	return PathChapterHandle;
}

void USpawnController::SpawnWeaponAtActor()
{
}

void USpawnController::SpawnActor(UObject* GivenActor, UWorld* CurrentWorld, FVector Position, FRotator Rotation)
{

	FActorSpawnParameters ActorParam;
	AActor* SpawnObject = CurrentWorld->SpawnActor<AActor>(GivenActor->GetClass(), Position, Rotation, ActorParam);
}

void USpawnController::SpawnCharacter(UObject* GivenCharacter, UWorld* CurrentWorld, FVector Position, FRotator Rotation)
{
	FActorSpawnParameters ActorParam;
	ABlasterCharacter* SpawnCharacter = CurrentWorld->SpawnActor<ABlasterCharacter>(GivenCharacter->GetClass(), Position, Rotation, ActorParam);
	if (SpawnCharacter && SpawnCharacter->IsAICharacter() && !SpawnCharacter->IsWeaponEquipped())
	{
		AAICharacterController* Controller = Cast<AAICharacterController>(SpawnCharacter->GetController());
	}
}

/**
* 	default: cube with range (string)
*	In current development, we will only get the location and rotation of current
*	Training Wall to create Range
*/
void USpawnController::SpawnRangeObject(UWorld* CurrentWorld, FVector Position, FRotator Rotation, int32 NoObjects)
{
	const float Range = 150.f;
	AActor* SpawnObject;
	//this is default Training Range Number, this can be change
	if (CurrentWorld)
	{
		FString Path = "/Script/Engine.Blueprint'/Game/BasicShapes/BP_Cube.BP_Cube'";
		UBlueprint* Cube = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *Path));
		if (Cube)
		{
			FActorSpawnParameters ObjectParam;
			for (int Objects = 1; Objects <= NoObjects; ++Objects)
			{
				SpawnObject = CurrentWorld->SpawnActor<AActor>(Cube->GeneratedClass, Position, Rotation, ObjectParam);
				if (SpawnObject)
				{
					UStaticMeshComponent* CubeMesh = Cast<UStaticMeshComponent>(SpawnObject->GetRootComponent());
					CubeMesh->SetRelativeScale3D(FVector(.10f, 1.f, 1.f));
					float RangeEach = (Range * Objects);
					GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString("Spawn"));
					SpawnObject->SetActorLocation(
						FVector(Position.X + RangeEach, Position.Y, 0)
					);
					DrawDebugString(CurrentWorld, SpawnObject->GetActorLocation(), FString::SanitizeFloat(RangeEach));
				}	//TODO: turn of string of the distance from character to training room
			}
			//TODO: Remind of Pointer that duplicated
		}
	}
}

UObject* USpawnController::SpawnActorByPathAndName(UWorld* CurrentWorld, FString Path, FString Name, FVector Position, FRotator Rotation)
{
	TArray<FPrimaryAssetId> ObjectIdList;

	TArray<UObject*> ObjectList;

	FPrimaryAssetType ObjectAssetType;

	UAssetManager& ActorManager = UAssetManager::Get();

	int32 bActorFound;
	//if (isBlueprintRequire)
	//{
	//	bActorFound = ActorManager.ScanPathForPrimaryAssets(
	//		ObjectAssetType = FPrimaryAssetType("Blueprint"),
	//		Path,
	//		UBlueprint::StaticClass(),
	//		true, true); //doesnt test yet
	//}
	bActorFound = ActorManager.ScanPathForPrimaryAssets(
		ObjectAssetType = FPrimaryAssetType("Object"),
		Path,
		UObject::StaticClass(),
		false, true);
	if (bActorFound != 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, FString::FromInt(bActorFound));
	}
	ActorManager.GetPrimaryAssetIdList(ObjectAssetType, ObjectIdList);
	ActorManager.GetPrimaryAssetObjectList(ObjectAssetType, ObjectList);
	for (const UObject* ObjectIdItem : ObjectList)
	{
		AHandAttachObject* NewObject = CurrentWorld->SpawnActor<AHandAttachObject>(ObjectIdItem->StaticClass());
		if (!NewObject)
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString("Failed to spawn"));
	}

	//for (const FPrimaryAssetId& ObjectIdItem : ObjectIdList)
	//{
	//	if (Name.Contains(ObjectIdItem.PrimaryAssetName.ToString()) == false) continue;

	//	FAssetData ObjectData;
	//	
	//	ActorManager.GetPrimaryAssetData(ObjectIdItem, ObjectData);
	//	UObject* CurrentObject = ObjectData.GetAsset();
	//	AHandAttachObject* CurrentActor = Cast<AHandAttachObject>(ObjectData.GetAsset());
	//	TSubclassOf<AHandAttachObject>HandAttach;
	//	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, FString(ObjectBP));
	//	//ActorParam->ClassFlags = EClassFlags::CLASS_DefaultToInstanced;

	//	FActorSpawnParameters ActorParam;
	//	FTransform ObjectTransform;
	//	
	//	SpawnActorAtLocation(Path, CurrentWorld, FVector(), FRotator());

	//	AHandAttachObject* NewObject = CurrentWorld->SpawnActor<AHandAttachObject>(CurrentObject->StaticClass());
	//	if (NewObject)
	//		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString(NewObject->GetName()));
	//	/*if (false)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString("Find current Object"));
	//		AHandAttachObject* NewObject = CurrentWorld->SpawnActor<AHandAttachObject>(ObjectData.GetClass()->GetClass(), FVector(0), FRotator(0));
	//		if(NewObject)
	//			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString(NewObject->GetName()));
	//	}*/

	//	//if(CurrentObject)
	//	//	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, FString(CurrentObject->GetPathName()));
	//	
	//	//return CurrentWorld->SpawnActor<UObject>(CurrentObjectBP->GeneratedClass, FVector(0), FRotator(0));
	//}

	return nullptr;

}

AHandAttachObject* USpawnController::SpawnActorHandAttachByPathName(UWorld* CurrentWorld, bool isBlueprintLocation, FString Path, FString Name, FVector Position, FRotator Rotation)
{
	FString MiddleLocation = isBlueprintLocation ? "Blueprints/" : "";
	FString FullPath = FString(
		"/Game/" +
		MiddleLocation +
		"Hand_Attach/" + Path + +"/" + Name);
	//UE_LOG(LogTemp, Warning, TEXT("Path AK56 : %s"), *FullPath);

	return Cast<AHandAttachObject>(SpawnActorWithValue(FullPath, CurrentWorld, Position, Rotation));
}

AHandAttachObject* USpawnController::SpawnActorHandAttachByActor(UWorld* CurrentWorld, AActor* HandAttachActor, FVector Position, FRotator Rotation)
{
	return Cast<AHandAttachObject>(SpawnActorAtLocationByActor(HandAttachActor, CurrentWorld, Position, Rotation));;
}

AActor* USpawnController::SpawnActorWithValue(FString Path, UWorld* CurrentWorld, FVector Position, FRotator Rotation)
{
	UE_LOG(LogTemp, Warning, TEXT("Path: %s"), *Path);
	UBlueprint* BPActor = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *Path));

	if (BPActor == nullptr || !BPActor->GeneratedClass->IsChildOf(AActor::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Load BP Fail"));
		return nullptr;
	}
	else
	{
		FActorSpawnParameters ActorParam;
		UWorld* MyWorld = CurrentWorld;
		FVector SpawnPosition = Position;
		FRotator SpawnRotation = Rotation;

		if (BPActor->GetName().Contains("Herlobcho") ||
			BPActor->GetName().Contains("CarbinePearls"))
		{
			ABlasterCharacter* SpawnCharacter = MyWorld->SpawnActor<ABlasterCharacter>(BPActor->GeneratedClass, SpawnPosition, SpawnRotation, ActorParam);
			if (SpawnCharacter && SpawnCharacter->IsAICharacter() && !SpawnCharacter->IsWeaponEquipped())
			{

				AAICharacterController* Controller = Cast<AAICharacterController>(SpawnCharacter->GetController());
				//if (Controller->GetRandomWeaponAttach())
				//{
				//	Controller->AttachWeapon();
				//	 
				//}
			}
			return SpawnCharacter;
		}
		else
			return MyWorld->SpawnActor<AActor>(BPActor->GeneratedClass, SpawnPosition, SpawnRotation, ActorParam);

	}
	return nullptr;
}

ANotification* USpawnController::SpawnNotification(FString Path, UWorld* CurrentWorld, FVector Position, FRotator Rotation)
{
	UE_LOG(LogTemp, Warning, TEXT("Trigger Notification Path: %s"), *Path);
	UBlueprint* BPActor = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *Path));

	if (BPActor == nullptr || !BPActor->GeneratedClass->IsChildOf(AActor::StaticClass()))
	{
		return nullptr;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString("Success Spawn"));
		FActorSpawnParameters ActorParam;
		return CurrentWorld->SpawnActor<ANotification>(BPActor->GeneratedClass, Position, Rotation, ActorParam);

	}

}

ANotification* USpawnController::SpawnNotification(AActor* GivenActor, UWorld* CurrentWorld, FVector Position, FRotator Rotation)
{
	if (GivenActor == nullptr) return nullptr;

	UClass* GivenClass = GivenActor->GetClass();

	if (GivenClass == nullptr) return nullptr;

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString("Success Spawn"));
	FActorSpawnParameters ActorParam;

	return CurrentWorld->SpawnActor<ANotification>(GivenClass, Position, Rotation, ActorParam);

}


TArray<FString> USpawnController::GetObjectPath(EScanObjectType Type)
{
	TArray<FAssetData> Datas = GetObjectLibrary(UEnum::GetDisplayValueAsText(Type).ToString());
	TArray<FString> ObjectDataPath;
	for (FAssetData Data : Datas)
	{
		FString ObjectBP = Data.AssetName.ToString();
		ObjectDataPath.Add(ObjectBP);
		UE_LOG(LogTemp, Warning, TEXT("Here is the expect: %s"), *FString(ObjectBP));
	}

	return ObjectDataPath;
}

void USpawnController::LoadAllObjectPath(EScanObjectType Type, TArray<FString>& LoadDatas)
{
	TArray<FAssetData> Datas = GetObjectLibrary(UEnum::GetDisplayValueAsText(Type).ToString());
	for (FAssetData Data : Datas)
	{
		FString ObjectBP = Data.AssetName.ToString();
		LoadDatas.Add(ObjectBP);
		UE_LOG(LogTemp, Warning, TEXT("Here is the expect: %s"), *FString(ObjectBP));
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(LoadDatas[0]));

}

TArray<FAssetData> USpawnController::GetObjectLibrary(FString ObjectName)
{
	TArray<FAssetData> AssetDatas;
	TArray<FAssetData> AllAssetDatas;

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(AActor::StaticClass(), true, GIsEditor);
	ObjectLibrary->AddToRoot();

	UE_LOG(LogTemp, Error, TEXT("ObjectName = %s"), *FString(ObjectName));

	if (ObjectName == "Weapon")
	{
		for (EWeaponType Val : TEnumRange<EWeaponType>())
		{
			FString BPName = UEnum::GetDisplayValueAsText(Val).ToString();
			LoadObjectPath(ObjectName, BPName, ObjectLibrary, AllAssetDatas);
		}
	}
	else if (ObjectName == "Pickups")
	{
		for (EPickupType Val : TEnumRange<EPickupType>())
		{
			FString BPName = UEnum::GetDisplayValueAsText(Val).ToString();
			LoadObjectPath(ObjectName, BPName, ObjectLibrary, AllAssetDatas);
		}
	}
	else if (ObjectName == "Character")
	{
		for (ECharacterType Val : TEnumRange<ECharacterType>())
		{
			FString BPName = UEnum::GetDisplayValueAsText(Val).ToString();
			LoadObjectPath(FString("AI/Character"), BPName, ObjectLibrary, AllAssetDatas);
		}
	}
	else if (ObjectName == "Ammo")
	{
		for (EPickupType Val : TEnumRange<EPickupType>())
		{
			FString BPName = UEnum::GetDisplayValueAsText(Val).ToString();
			LoadObjectPath("Pickups", "AmmoPickups", ObjectLibrary, AllAssetDatas);
		}
	}

	else if (ObjectName == "Health")
	{
		for (EPickupType Val : TEnumRange<EPickupType>())
		{
			FString BPName = UEnum::GetDisplayValueAsText(Val).ToString();
			LoadObjectPath("Pickups", "BuffPickups", ObjectLibrary, AllAssetDatas);
		}
	}
	//else if(ObjectName == )
	return AllAssetDatas;
}

void USpawnController::LoadObjectPath(FString ObjectName, FString BPName, UObjectLibrary* ObjectLibrary, TArray<FAssetData>& AllAssetDatas)
{
	TArray<FAssetData> AssetDatas;
	ObjectLibrary->LoadBlueprintAssetDataFromPath(TEXT("/Game/Blueprints/" + ObjectName + "/" + BPName));

	ObjectLibrary->GetAssetDataList(AssetDatas);

	for (FAssetData Data : AssetDatas)
	{
		Data.AssetName = FName(TEXT("" + ObjectName + "/" + BPName + "/" + Data.AssetName.ToString()));
		AllAssetDatas.Add(Data);
	}
}

void USpawnController::SpawnActorAtLocation(FString Path, UWorld* CurrentWorld, FVector Position, FRotator Rotation)
{
	UE_LOG(LogTemp, Warning, TEXT("Path: %s"), *Path);
	UBlueprint* BPActor = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *Path));

	if (BPActor == nullptr || !BPActor->GeneratedClass->IsChildOf(AActor::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Load BP Fail"));
	}
	else
	{
		FActorSpawnParameters ActorParam;
		UWorld* MyWorld = CurrentWorld;
		FVector SpawnPosition = Position;
		FRotator SpawnRotation = Rotation;

		if (BPActor->GetName().Contains("Herlobcho") ||
			BPActor->GetName().Contains("CarbinePearls"))
		{
			ABlasterCharacter* SpawnCharacter = MyWorld->SpawnActor<ABlasterCharacter>(BPActor->GeneratedClass, SpawnPosition, SpawnRotation, ActorParam);
			if (SpawnCharacter && SpawnCharacter->IsAICharacter() && !SpawnCharacter->IsWeaponEquipped())
			{

				AAICharacterController* Controller = Cast<AAICharacterController>(SpawnCharacter->GetController());

			}
		}
		else
			AActor* SpawnObject = MyWorld->SpawnActor<AActor>(BPActor->GeneratedClass, SpawnPosition, SpawnRotation, ActorParam);

	}
}

void USpawnController::SpawnActorAtLocation(AActor* GivenActor, UWorld* CurrentWorld, FVector Position, FRotator Rotation)
{
	if (GivenActor == nullptr) return;

	UClass* GivenClass = GivenActor->GetClass();

	if (GivenClass == nullptr) return;

	FActorSpawnParameters ActorParam;
	UWorld* MyWorld = CurrentWorld;
	FVector SpawnPosition = Position;
	FRotator SpawnRotation = Rotation;

	if (GivenClass->GetName().Contains("Herlobcho") ||
		GivenClass->GetName().Contains("CarbinePearls"))
	{
		ABlasterCharacter* SpawnCharacter = MyWorld->SpawnActor<ABlasterCharacter>(GivenClass, SpawnPosition, SpawnRotation, ActorParam);
	}
	else
		AActor* SpawnObject = MyWorld->SpawnActor<AActor>(GivenClass, SpawnPosition, SpawnRotation, ActorParam);


}

AActor* USpawnController::SpawnActorAtLocationByActor(AActor* GivenActor, UWorld* CurrentWorld, FVector Position, FRotator Rotation)
{
	if (GivenActor == nullptr) return nullptr;

	UClass* GivenClass = GivenActor->GetClass();

	if (GivenClass == nullptr) return nullptr;

	FActorSpawnParameters ActorParam;
	UWorld* MyWorld = CurrentWorld;
	FVector SpawnPosition = Position;
	FRotator SpawnRotation = Rotation;

	if (GivenClass->GetName().Contains("Herlobcho") ||
		GivenClass->GetName().Contains("CarbinePearls"))
	{
		ABlasterCharacter* SpawnCharacter = MyWorld->SpawnActor<ABlasterCharacter>(GivenClass, SpawnPosition, SpawnRotation, ActorParam);
		if (SpawnCharacter && SpawnCharacter->IsAICharacter() && !SpawnCharacter->IsWeaponEquipped())
		{
			AAICharacterController* Controller = Cast<AAICharacterController>(SpawnCharacter->GetController());
		}
		return SpawnCharacter;
	}
	else
		return MyWorld->SpawnActor<AActor>(GivenClass, SpawnPosition, SpawnRotation, ActorParam);

}



