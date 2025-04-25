#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "Blaster/BlasterTypes/Constant.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "Blaster/PathChapter/PathChapter.h"
#include "Blaster/Objects/Notification.h"
#include "Blaster/Spawn/SpawnInitializeComponent.h"
#include "BlasterGameMode.generated.h"


UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ABlasterGameMode();
	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	
	void ReloadEditorObject();
	void ReloadObject();

	void ReloadPathChapterDataTable();
	void SpawnObjectRandom();
	void SpawnEditorObject(EScanObjectType ObjectType);
	void SpawnObject(EScanObjectType ObjectType);
	void SpawnEditorObjectAtLocation(FVector Location, EScanObjectType ObjectType);
	void SpawnObjectAtLocation(FVector Location, EScanObjectType ObjectType);
	void DisplayRangeTraining();


	FVector GetRandomLocationAtMap();
	FRotator GetRandomRotation();
	
	/**
	* Path for item (Editor only)
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString>CharacterDataPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString>WeaponDataPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString>PickupDataPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString>ItemDataPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString>AmmoDataPath;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString>HealthDataPath;


	/**
	* Item for spawning, load onbject
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*>CharacterData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*>WeaponData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*>AmmoAttachData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*>MagAttachData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*>AmmoData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*>HealthData;

	/**
	* Path Chapter
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*>TriggerData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*>PathChapterData;

	/**
	* Weapon HUD
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*>ImageData;

	/**
	* Other
	*/

	AActor* NotificationData;
	FString NotificationDataPath; //Cannot using right now

private:
	UPROPERTY(VisibleAnywhere)
	class USpawnController* SpawnController;

	UPROPERTY(EditAnywhere)
	class USpawnInitializeComponent* SpawnInitializeComponent;

	float KillCount = 1.f; 

	TMap<FVector, FVector> FloorMap;

	UPROPERTY(VisibleAnywhere)
	FVector MapLocation;
	
	UPROPERTY(VisibleAnywhere)
	FVector MapExtent;

	FVector MainMapLocation;
	FVector MainMapExtent;
	FVector TrainingWallLocation;
	FRotator TrainingWallRotator;

	UPROPERTY(EditAnywhere)
	float SpawnTimer = DEFAULT_SPAWN_TIMER_EASY;

	float FloorMaxX;
	float FloorMinX;
	float FloorMaxY;
	float FloorMinY;


	UPROPERTY(VisibleAnywhere)
	UPathChapter* CurrentPathChapter;

	FDataTableRowHandle TableHandle;

	APawn* PlayerPawn;

	int32 CurrentCharacterInMap;
	int32 CurrentWeaponItemInMap;
	int32 CurrentWeaponInMap;
	int32 CurrentAmmoInMap;
	int32 CurrentHealthInMap;

	bool bFoundTrainingWall = false;
protected:

public:
	bool IsCollisionAtMap(FVector GivenLocation);
	bool IsCollisionAtMap(UObject* GivenObject);

	int32 GetCharacterAvailableInMap();

	UPathChapter* GetPathChapter();

	FString GetObjectPath(EScanObjectType ObjectType);
	AActor* GetActorByData(EScanObjectType ObjectType);

	FVector GetRandomLocationAtFloorMap();
	bool CheckSpawnCharacter();
	bool CheckSpawnWeaponItem();
	bool CheckSpawnWeapon();
	bool CheckSpawnAmmo();
	bool CheckSpawnHealth();
	//TODO: need using spawn in SpawnController

	int32 DecreaseCharacterNum();
	int32 DecreaseWeaponItemNum();
	int32 DecreaseWeaponNum();
	int32 DecreaseAmmoNum();
	int32 DecreaseHealthNum();

	int32 IncreaseCharacterNum();
	int32 IncreaseWeaponItemNum();
	int32 IncreaseWeaponNum();
	int32 IncreaseAmmoNum();
	int32 IncreaseHealthNum();

	FORCEINLINE FString GetNotificationDataPath() const { return NotificationDataPath; }
	FORCEINLINE AActor* GetNotificationData() const { return NotificationData; }
	FORCEINLINE APawn* GetPlayerPawn() const { return PlayerPawn; }
	FORCEINLINE FDataTableRowHandle GetTableHandle() const { return TableHandle; }
};
