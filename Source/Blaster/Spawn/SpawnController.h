#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/SoftObjectPtr.h"

#include "Engine/ObjectLibrary.h"
#include "Engine/DataAsset.h"
#include "Engine/StaticMesh.h"

#include "Blaster/Objects/HandAttachObject.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Objects/Notification.h"
#include "Blaster/Spawn/SpawnInitializeComponent.h"

#include "SpawnController.generated.h"

class UTexture2D;
class USpawnInitializeComponent;
/**
 * 
 */
UCLASS()
class BLASTER_API USpawnController : public UObject
{
	GENERATED_BODY()
public:
	USpawnController();

	//below all function is the older version of getting data, please use it editor only 
	//if use for needed, careful for performance issue
	UFUNCTION(BlueprintCallable, Category = "BlueprintsLibrary")
	static TArray<FString> GetObjectPath(EScanObjectType Type);

	UFUNCTION(BlueprintCallable, Category = "BlueprintsLibrary")
	static TArray<UTexture2D*> LoadWeaponImage();

	static UTexture2D* LoadSingleWeaponImage();

	static FString LoadNotification();

	void LoadAllObjectPath(EScanObjectType Type, TArray<FString>& LoadDatas);
	
	static void SpawnActorAtLocation(FString Path, UWorld* CurrentWorld, FVector Position, FRotator Rotation); 
	static void SpawnActorAtLocation(AActor* GivenActor, UWorld* CurrentWorld, FVector Position, FRotator Rotation);
	static AActor* SpawnActorAtLocationByActor(AActor* GivenActor, UWorld* CurrentWorld, FVector Position, FRotator Rotation);

	static void SpawnActor(UObject* GivenActor, UWorld* CurrentWorld, FVector Position, FRotator Rotation);
	static void SpawnCharacter(UObject* GivenCharacter, UWorld* CurrentWorld, FVector Position, FRotator Rotation);
	static void SpawnRangeObject(UWorld* CurrentWorld, FVector Position, FRotator Rotation, int32 NoObjects);


	static UObject* SpawnActorByPathAndName(UWorld* CurrentWorld, 
		FString Path, 
		FString Name, 
		FVector Position, 
		FRotator Rotation);
	static AHandAttachObject* SpawnActorHandAttachByPathName(
		UWorld* CurrentWorld, 
		bool isBlueprintLocation,
		FString Path, 
		FString Name, 
		FVector Position, 
		FRotator Rotation);

	static AHandAttachObject* SpawnActorHandAttachByActor(
		UWorld* CurrentWorld,
		AActor* HandAttachActor,
		FVector Position,
		FRotator Rotation);


	static AActor* SpawnActorWithValue(FString Path, UWorld* CurrentWorld, FVector Position, FRotator Rotation);

	//TODO: write UBlueprunt SpawnActor when bool isBlueprintRequire = true

	static ANotification* SpawnNotification(FString Path, UWorld* CurrentWorld, FVector Position, FRotator Rotation);
	static ANotification* SpawnNotification(AActor* GivenActor, UWorld* CurrentWorld, FVector Position, FRotator Rotation);

	static FDataTableRowHandle GetEditorPathChapterDataTable();
	TArray<FString> PublicObjectDataPath;

	void SpawnWeaponAtActor();

private: 
	//this can be use for both older and newer version
	UPROPERTY(VisibleAnywhere)
	int32 AssetIndex = 0;

	static TArray<FAssetData> GetObjectLibrary(FString ObjectName);

	static void LoadObjectPath(FString ObjectName, FString BPName, UObjectLibrary* ObjectLibrary, TArray<FAssetData>& AllAssetDatas);

	//fix this
	//Note that some function call new class still leak 
	//and need to stop scan after done

	//new version only down below

public:
	USpawnInitializeComponent* SpawnInitializeComponent;

	//FDataTableRowHandle GetPathChapterDataTable();
	
private:

};
