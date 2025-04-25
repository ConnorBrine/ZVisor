#include "BlasterGameMode.h"
#include "EngineUtils.h"

#include "Engine/DataTable.h"
#include "Engine/OverlapResult.h"

#include "GameFramework/GameStateBase.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Spawn/SpawnController.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/Objects/PickupType.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/PathChapter/PathChapter.h" 
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Pickup/Pickup.h"
#include "Blaster/Pickup/AmmoPickup.h"
#include "Blaster/Pickup/HealthPickup.h"

ABlasterGameMode::ABlasterGameMode()
{
	//CurrentPathChapter = CreateDefaultSubobject<UPathChapter>(TEXT("PathChapter"));
}

//if(Itr->GetActorLabel().Contains(FString("Floor")))
//{
//	//UE_LOG(LogTemp, Warning, TEXT("Floor: %s"), *Itr->GetName());
//	UKismetSystemLibrary::GetActorBounds(*Itr, MapLocation, MapExtent);
//	FloorMap.Emplace(MapLocation, MapExtent);
//}
void ABlasterGameMode::StartPlay()
{	
	ReloadObject();

	/*GetWorld()->WorldType == EWorldType::PIE ?
		ReloadEditorObject() : ReloadObject();*/
	//ReloadPathChapterDataTable();

	for (TObjectIterator<AActor> Itr; Itr; ++Itr)
	{
		//if (!bFoundTrainingWall && Itr->GetActorLabel().Contains(FString("TrainingWall")))
		//{
		//	//UKismetSystemLibrary::GetActorBounds(*Itr, MapLocation, MapExtent);
		//	TrainingWallLocation = Itr->GetActorLocation();
		//	TrainingWallRotator = Itr->GetActorRotation();
		//	DisplayRangeTraining();
		//	bFoundTrainingWall = true;
		//}
		//training in testung, in product this will not show

		if (Itr->GetName().Contains(FString("Floor")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Itr: %s"), *Itr->GetName());
			UKismetSystemLibrary::GetActorBounds(*Itr, MainMapLocation, MainMapExtent);
		}
	}

	FloorMaxX = MainMapLocation.X + MainMapExtent.X;
	FloorMaxY = MainMapLocation.Y + MainMapExtent.Y;
	FloorMinX = MainMapLocation.X - MainMapExtent.X;
	FloorMinY = MainMapLocation.Y - MainMapExtent.Y;

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABlasterGameMode::SpawnObjectRandom, SpawnTimer, true, 5.f);

	CurrentCharacterInMap = 0;
	CurrentWeaponItemInMap = 0;

	for (TActorIterator<ABlasterCharacter> Character(GetWorld()); Character; ++Character)
	{
		++CurrentCharacterInMap;
	}
	UE_LOG(LogTemp, Warning, TEXT("Character begin Play = %d"), CurrentCharacterInMap);

	for (TActorIterator<AWeapon> Weapon(GetWorld()); Weapon; ++Weapon)
	{
		++CurrentWeaponInMap;
		++CurrentWeaponItemInMap;
	}
	UE_LOG(LogTemp, Warning, TEXT("Weapon begin Play = %d"), CurrentWeaponInMap);
	for (TActorIterator<AAmmoPickup> Ammo(GetWorld()); Ammo; ++Ammo)
	{
		++CurrentAmmoInMap;
		++CurrentWeaponItemInMap;
	}
	UE_LOG(LogTemp, Warning, TEXT("Ammo begin Play = %d"), CurrentAmmoInMap);
	for (TActorIterator<AHealthPickup> Health(GetWorld()); Health; ++Health)
	{
		++CurrentHealthInMap;
		++CurrentWeaponItemInMap;
	}
	UE_LOG(LogTemp, Warning, TEXT("Health begin Play = %d"), CurrentHealthInMap);
	UE_LOG(LogTemp, Warning, TEXT("Item begin Play = %d"), CurrentWeaponItemInMap);

	Super::StartPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ABlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	GEngine->AddOnScreenDebugMessage(0, 3, FColor::Yellow, FString::FromInt(CurrentCharacterInMap));
}


void ABlasterGameMode::ReloadEditorObject()
{
	TArray<FString>Data = USpawnController::GetObjectPath(EScanObjectType::ESOT_Character);
	ImageData = USpawnController::LoadWeaponImage();
	CharacterDataPath = Data;

	Data.Reset();
	Data = USpawnController::GetObjectPath(EScanObjectType::ESOT_Weapon);
	WeaponDataPath = Data;

	Data.Reset();
	Data = USpawnController::GetObjectPath(EScanObjectType::ESOT_Pickups);
	PickupDataPath = Data;
	
	Data.Reset();
	Data = USpawnController::GetObjectPath(EScanObjectType::ESOP_Ammo);
	AmmoDataPath = Data;

	Data.Reset();
	Data = USpawnController::GetObjectPath(EScanObjectType::ESOP_Health);
	HealthDataPath = Data;

	NotificationDataPath = USpawnController::LoadNotification(); //BUG
	
	//editor will not use TableHandle as finding, it will call USpawnComponent instead
	
}

void ABlasterGameMode::ReloadObject()
{
	ImageData = SpawnInitializeComponent->LoadWeaponHUD();

	CharacterData = SpawnInitializeComponent->LoadActor(EScanObjectType::ESOT_Character);
	WeaponData = SpawnInitializeComponent->LoadActor(EScanObjectType::ESOT_Weapon);
	AmmoAttachData = SpawnInitializeComponent->LoadActor(EScanObjectType::ESOP_AmmoAttach);
	MagAttachData = SpawnInitializeComponent->LoadActor(EScanObjectType::ESOP_MagAttach);
	AmmoData = SpawnInitializeComponent->LoadActor(EScanObjectType::ESOP_Ammo);
	HealthData = SpawnInitializeComponent->LoadActor(EScanObjectType::ESOP_Health);

	TriggerData = SpawnInitializeComponent->LoadActor(EScanObjectType::ESOP_Trigger);
	PathChapterData = SpawnInitializeComponent->LoadActor(EScanObjectType::ESOP_PathChapterItem);

	NotificationData = SpawnInitializeComponent->LoadNotification();

	TableHandle = SpawnInitializeComponent->LoadPathChapterHandle();
}

void ABlasterGameMode::ReloadPathChapterDataTable()
{
	//TODO: This is reload data from all datatable so after the mid term will change this later as all the map
	//active only if player activate objective, not the random objective from there to widget

	//FString RowName = FString("PC" + FString::FromInt(FMath::RandRange(1, 3)));
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString(RowName));

	/*PathChapterHandle = USpawnController::GetPathChapterDataTable();
	ObjectivePathChapter = PathChapterHandle.DataTable->FindRow<FPathChapterDetail>("PC4", "");*/
	
	if (CurrentPathChapter)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString("Is Reading PC1"));
		CurrentPathChapter->ReloadRowDataTable("PC1");
		//PathChapter->ReloadRowDataTable("PC2");
		//PathChapter->ReloadRowDataTable("PC3");
		//PathChapter->ReloadRowDataTable("PC4");
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString(CurrentPathChapter->GetPathChapterDetail()->Name));
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString(CurrentPathChapter->GetPathChapterDetail()->Description));
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString(PathChapter->GetPathChapterDetail()->Description));
}

void ABlasterGameMode::SpawnObjectRandom()
{
	switch (int32 RandObj = FMath::RandRange(1, 4))
	{
	case 1:
		if (CheckSpawnCharacter())
		{
			//GetWorld()->WorldType == EWorldType::PIE ?
			//	SpawnEditorObject(EScanObjectType::ESOT_Character) : 
				SpawnObject(EScanObjectType::ESOT_Character);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("No more Character"));
		break;
	case 2:
		if (CheckSpawnWeaponItem())
		{
			//GetWorld()->WorldType == EWorldType::PIE ?
			//	SpawnEditorObject(EScanObjectType::ESOT_Weapon) : 
				SpawnObject(EScanObjectType::ESOT_Weapon);
		}
			
		else
			UE_LOG(LogTemp, Warning, TEXT("No more Item Weapon"));
		break;
	case 3:
		if (CheckSpawnAmmo())
		{
			//GetWorld()->WorldType == EWorldType::PIE ?
			//	SpawnEditorObject(EScanObjectType::ESOP_Ammo) :
				SpawnObject(EScanObjectType::ESOP_Ammo);
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("No more Ammo"));
		break;
	case 4:
		if (CheckSpawnHealth())
			//GetWorld()->WorldType == EWorldType::PIE ?
			//SpawnEditorObject(EScanObjectType::ESOP_Health) :
			SpawnObject(EScanObjectType::ESOP_Health);
		else
			UE_LOG(LogTemp, Warning, TEXT("No more Health"));
		break;


	case -1: //test for cases if passed or not
		if (CheckSpawnWeaponItem())
			SpawnObject(EScanObjectType::ESOT_Pickups);
		else
			UE_LOG(LogTemp, Warning, TEXT("No more Item Weapon"));
		break;
	}
}

void ABlasterGameMode::SpawnObject(EScanObjectType ObjectType)
{
	AActor* ActorObject = GetActorByData(ObjectType);

	USpawnController::SpawnActorAtLocation(
		ActorObject,
		GetWorld(),
		GetRandomLocationAtFloorMap(),
		GetRandomRotation());
}
void ABlasterGameMode::SpawnEditorObject(EScanObjectType ObjectType)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString("Spawned"));
	FString ObjectPath = GetObjectPath(ObjectType);

	USpawnController::SpawnActorAtLocation(
		ObjectPath,
		GetWorld(),
		GetRandomLocationAtFloorMap(),
		GetRandomRotation());
}

void ABlasterGameMode::SpawnObjectAtLocation(FVector Location, EScanObjectType ObjectType)
{
	AActor* ActorObject = GetActorByData(ObjectType);

	USpawnController::SpawnActorAtLocation(
		ActorObject,
		GetWorld(),
		Location,
		GetRandomRotation());
}

void ABlasterGameMode::SpawnEditorObjectAtLocation(FVector Location, EScanObjectType ObjectType)
{
	FString ObjectPath = GetObjectPath(ObjectType);

	USpawnController::SpawnActorAtLocation(
		ObjectPath,
		GetWorld(),
		Location,
		GetRandomRotation());
}
/**
* This only apply with testing
* In the future development, we will display as Training Room by this prototype
*/
void ABlasterGameMode::DisplayRangeTraining()
{
	//USpawnController::SpawnRangeObject(GetWorld(), TrainingWallLocation, TrainingWallRotator, 4);
}



FVector ABlasterGameMode::GetRandomLocationAtMap()
{
	return FVector(
		FMath::FRandRange(FloorMinX, FloorMaxX),
		FMath::FRandRange(FloorMinY, FloorMaxY), 
		100.f);
}

FRotator ABlasterGameMode::GetRandomRotation()
{
	return FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f);
}

bool ABlasterGameMode::IsCollisionAtMap(FVector GivenLocation)
{
	bool bHit = false;
	TArray<FOverlapResult>OverlapResult;
	GetWorld()->OverlapMultiByChannel(OverlapResult, GivenLocation, FQuat(), ECollisionChannel::ECC_Camera, FCollisionShape::MakeBox(FVector3f(100.f, 100.f, 100.f)));

	FString NameResult = "";
	for (const FOverlapResult Result: OverlapResult)
	{ 
		NameResult = FString(NameResult + " " + FString(Result.GetActor()->GetName()));
		if (!Result.GetActor()->GetName().Contains(FString("Floor")))
		{
			bHit = true;
			break;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Hit by: %s"), *NameResult);
	return bHit;
}

bool ABlasterGameMode::IsCollisionAtMap(UObject* GivenObject)
{
	return false;
}

int32 ABlasterGameMode::GetCharacterAvailableInMap()
{
	return GetWorld()->GetActorCount();
}

UPathChapter* ABlasterGameMode::GetPathChapter()
{
	return CurrentPathChapter;
}

FString ABlasterGameMode::GetObjectPath(EScanObjectType ObjectType)
{
	FString ObjectPath = "";
	int32 ObjectIndex = 0;

	switch (ObjectType)
	{
	case EScanObjectType::ESOT_Character:
		ObjectIndex = FMath::RandRange(0, CharacterDataPath.Num() - 1);
		if (!CharacterDataPath.IsEmpty() && CharacterDataPath.IsValidIndex(ObjectIndex))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(CharacterData[ObjectIndex]));
			++CurrentCharacterInMap;		
			return TEXT("/Game/Blueprints/" + CharacterDataPath[ObjectIndex]);
		}
		break;

	case EScanObjectType::ESOT_Weapon:
		ObjectIndex = FMath::RandRange(0, WeaponDataPath.Num() - 1);
		if (!WeaponDataPath.IsEmpty() && WeaponDataPath.IsValidIndex(ObjectIndex))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(WeaponData[ObjectIndex]));
			++CurrentWeaponItemInMap;
			return TEXT("/Game/Blueprints/" + WeaponDataPath[ObjectIndex]);
		}
		break;

	case EScanObjectType::ESOP_Ammo:
		ObjectIndex = FMath::RandRange(0, AmmoDataPath.Num() - 1);
		if (!AmmoDataPath.IsEmpty() && AmmoDataPath.IsValidIndex(ObjectIndex))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(WeaponData[ObjectIndex]));
			++CurrentAmmoInMap;
			return TEXT("/Game/Blueprints/" + AmmoDataPath[ObjectIndex]);
		}
		break;

	case EScanObjectType::ESOP_Health:
			ObjectIndex = FMath::RandRange(0, HealthDataPath.Num() - 1);
			if (!HealthDataPath.IsEmpty() && HealthDataPath.IsValidIndex(ObjectIndex))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(WeaponData[ObjectIndex]));
				++CurrentHealthInMap;
				return TEXT("/Game/Blueprints/" + HealthDataPath[ObjectIndex]);
			}
			break;

	case EScanObjectType::ESOT_Pickups:
		ObjectIndex = FMath::RandRange(0, PickupDataPath.Num() - 1);
		if (!PickupDataPath.IsEmpty() && PickupDataPath.IsValidIndex(ObjectIndex))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(PickupData[ObjectIndex]));
			++CurrentWeaponItemInMap;
			return TEXT("/Game/Blueprints/" + PickupDataPath[ObjectIndex]);
		}
		break;

	}
	return FString();
}

AActor* ABlasterGameMode::GetActorByData(EScanObjectType ObjectType)
{
	int32 ObjectIndex = 0;

	switch (ObjectType)
	{
	case EScanObjectType::ESOT_Character:
		ObjectIndex = FMath::RandRange(0, CharacterData.Num() - 1);
		if (!CharacterData.IsEmpty() && CharacterData.IsValidIndex(ObjectIndex))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(CharacterData[ObjectIndex]));
			++CurrentCharacterInMap;
			return CharacterData[ObjectIndex];
		}
		break;

	case EScanObjectType::ESOT_Weapon:
		ObjectIndex = FMath::RandRange(0, WeaponData.Num() - 1);
		if (!WeaponData.IsEmpty() && WeaponData.IsValidIndex(ObjectIndex))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(WeaponData[ObjectIndex]));
			++CurrentWeaponItemInMap;
			return  WeaponData[ObjectIndex];
		}
		break;

	case EScanObjectType::ESOP_Ammo:
		ObjectIndex = FMath::RandRange(0, AmmoData.Num() - 1);
		if (!AmmoData.IsEmpty() && AmmoData.IsValidIndex(ObjectIndex))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(WeaponData[ObjectIndex]));
			++CurrentAmmoInMap;
			return  AmmoData[ObjectIndex];
		}
		break;

	case EScanObjectType::ESOP_Health:
		ObjectIndex = FMath::RandRange(0, HealthData.Num() - 1);
		if (!HealthData.IsEmpty() && HealthData.IsValidIndex(ObjectIndex))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString(WeaponData[ObjectIndex]));
			++CurrentHealthInMap;
			return  HealthData[ObjectIndex];
		}
		break;

	//no pickup for final development 

	}
	return nullptr;
}

FVector ABlasterGameMode::GetRandomLocationAtFloorMap()
{
	FVector RandLocation;
	do
	{
		RandLocation = GetRandomLocationAtMap();
	} while (IsCollisionAtMap(RandLocation));

	return RandLocation;
}

bool ABlasterGameMode::CheckSpawnCharacter()
{
	return CurrentCharacterInMap <= MAX_SPAWN_CHARACTER;
}

bool ABlasterGameMode::CheckSpawnWeaponItem()
{
	return CurrentWeaponItemInMap <= MAX_SPAWN_WEAPON_ITEM;
}

bool ABlasterGameMode::CheckSpawnWeapon()
{
	return CurrentWeaponInMap <= MAX_SPAWN_WEAPON;
}

bool ABlasterGameMode::CheckSpawnAmmo()
{
	return CurrentAmmoInMap <= MAX_SPAWN_HEALTH;
}

bool ABlasterGameMode::CheckSpawnHealth()
{
	return CurrentHealthInMap <= MAX_SPAWN_AMMO;
}

int32 ABlasterGameMode::DecreaseCharacterNum()
{
	return --CurrentCharacterInMap;
}

int32 ABlasterGameMode::DecreaseWeaponItemNum()
{
	return --CurrentWeaponItemInMap;
}

int32 ABlasterGameMode::DecreaseWeaponNum()
{
	return --CurrentWeaponInMap;
}

int32 ABlasterGameMode::DecreaseAmmoNum()
{
	return --CurrentAmmoInMap;
}

int32 ABlasterGameMode::DecreaseHealthNum()
{
	return --CurrentHealthInMap;
}

int32 ABlasterGameMode::IncreaseCharacterNum()
{
	return  ++CurrentCharacterInMap;
}

int32 ABlasterGameMode::IncreaseWeaponItemNum()
{
	return  ++CurrentWeaponItemInMap;
}

int32 ABlasterGameMode::IncreaseWeaponNum()
{
	return ++CurrentWeaponInMap;
}

int32 ABlasterGameMode::IncreaseAmmoNum()
{
	return ++CurrentAmmoInMap;
}

int32 ABlasterGameMode::IncreaseHealthNum()
{
	return ++CurrentHealthInMap;
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	bool isPlayerController = VictimController ? true : false;
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;

	if (ElimmedCharacter)
	{
		if (!ElimmedCharacter->IsElimmed())
		{
			if (isPlayerController)
			{
				ABlasterPlayerState* VictimPlayerState = AttackerController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

				if (AttackerPlayerState && VictimPlayerState && AttackerPlayerState != VictimPlayerState)
				{
					AttackerPlayerState->AddToKills(1.f);
				}
			}
			else if (AttackerPlayerState)
			{
				AttackerPlayerState->AddToKills(1.f);
			}
		}

		ElimmedCharacter->Elim();
		ElimmedCharacter->SetHealth(0.f); //elim->death with this mid term project

	}

	//ABlasterPlayerController
	//AttackerPlayerState->AddToKills(1.f); //will fix this later



}

