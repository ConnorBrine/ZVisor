#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Blaster/Objects/Trigger/Trigger.h"
#include "Blaster/Objects/Notification.h"
#include "Blaster/PathChapter/ClearCondition.h"
#include "Blaster/Pickup/PathChapterItemPickup.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/BlasterTypes/Constant.h"
#include "Blaster/PathChapter/PathChapter.h"

#include "CharacterPathChapterComponent.generated.h"

class ABlasterCharacter;
class ABlasterPlayerState;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCharacterPathChapterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterPathChapterComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	friend ABlasterCharacter;

	/**
	* Initialize
	*/
	void PathChapterInit();
	void CaptureTimerInit();

	/**
	* General Setup
	*/
	//using for first time path chapter
	void ActivePathChapter();

	void SetupNewObjective();
	void UpdateObjective();
	void LoadObjective();

	void RemoveOldObjective(); 

	void UpdateCharacterPathChapterHUD();

	/**
	* Trigger (Travel / Capture)
	*/
	void TriggerPathChapter(FString CollisionName); 

	void UpdateCaptureTimer();
	void IncreaseCaptureTimer();
	void ResetCaptureTimer();
	void ClearTriggersData();

	void LoadTriggers(
		FString TriggerName,
		bool bSamePlace,
		int32 NumOfTrigger);


	/**
	* Path Chapter Item (Collect / Kill Collect)
	*/
	void LoadPathChapterItem(
		TSubclassOf<APathChapterItemPickup> Item,
		int32 NumItemInMap,
		bool bSameTriggerPlace = false,
		FString TriggerName = " ",
		FVector LocationForSpawn = FVector());

	void PickUpObjectiveItem(); //collect
	void AddToKillWithObjective(AActor* DamageCharacter); //kill collect
	void DecreasePickUpRequireObjective();

	/**
	* Other
	*/
	void SetPathChapterLocation(FVector LocationForSpawn);
	void ShowItemNotificationScreen(bool bVisibility);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	AActor* ObjectiveLocation;

	UPROPERTY(BlueprintReadOnly)
	FVector ObjectiveLocationVector;

private:
	/**
	* General
	*/
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	ABlasterPlayerState* BlasterPlayerState;

	UPROPERTY()
	UPathChapter* PathChapter;

	/**
	* Path Chapter Data Save
	*/
	bool bActivePathChapter = false;

	bool bObjectiveCheck = false;

	int32 NoObjectiveRemain = 0; //collect, collect kill, capture

	EClearCondition CurrentClearCondition = EClearCondition::ECC_Default;

	int32 CurrentObjectiveNum = -1;  //Default is -1 to determine data of path chapter not given

	/**
	* Capture / Travel
	*/

	//this is the data save for capture timer increase
	float BaseIncreaseCaptureTimer = 0.f;

	//Current Character Capture Time
	float CharacterCaptureTimer = 0.f;

	bool bOverlappedCapture = false;
	FString LatestCaptureTrigger = "empty";

	int32 TriggersPoint = 0;
	TArray<int32> TriggersPointArray;
	TArray<FString> TriggersName;
	TArray<ATrigger*> Triggers;

	/**
	* Collect
	*/
	bool bInCollectObjective = false; //collect

	/*
	* Kill Collect (in progress)
	*/
	bool IsInKillCollectState = false; //collect kill

	/**
	* Notification
	*/
	TArray<ANotification*> Notification;


	/**
	* Timer Handle
	*/

	FTimerHandle ObjectiveHandle;
	FTimerHandle ObjectiveHUDHandle;
	FTimerHandle ObjectiveHUDNumRequire;
	FTimerHandle LoadObjectFirstTimerHandle;

	/**
	* Other
	*/

public:	
	FORCEINLINE float GetCharacterCaptureTimer() const { return CharacterCaptureTimer; }

	FORCEINLINE int32 GetNoObjectiveRemain() const { return NoObjectiveRemain; }

	FORCEINLINE bool IsInCollectObjective() const { return bInCollectObjective; }
		
};
