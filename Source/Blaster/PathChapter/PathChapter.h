#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "UObject/NoExportTypes.h"
#include "Blaster/Pickup/PathChapterItemPickup.h"
#include "Blaster/Objects/Trigger/Trigger.h"
#include "Blaster/Path/IdlePath.h"
#include "Blaster/PathChapter/ClearCondition.h"
#include "PathChapter.generated.h"
/**
 * 
 */

UENUM(BlueprintType)
enum class EPathChaperReward : uint8
{
	EPCR_Default UMETA(DisplayName = "Default"),
	EPCR_Experience UMETA(DisplayName = "Experience"),
	EPCR_Itme UMETA(DisplayName = "Item"),

};


UENUM(BlueprintType)
enum class EPathChapterType : uint8
{
	EPCT_Main UMETA(DisplayName = "Main Story Path"),
	EPCT_Side UMETA(DisplayName = "Side Path"),
	EPCT_Other UMETA(DisplayName = "Other Path"),

};

USTRUCT(BlueprintType)
struct BLASTER_API FReward
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPathChaperReward RewardType = EPathChaperReward::EPCR_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> Item;
	//TODO: Item reward => aactor item 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EXP = 0.f;

private:
};

USTRUCT(BlueprintType)
struct BLASTER_API FObjective
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EClearCondition ClearType = EClearCondition::ECC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABlasterCharacter> EnemySlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APathChapterItemPickup> ItemToCollect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSamePlace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TriggerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DescriptionStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DescriptionEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ObjectiveID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumRequired = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CaptureTimer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsComplete = false;

private:

};

USTRUCT(BlueprintType)
struct BLASTER_API FPathChapterDetail : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPathChapterType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FReward Reward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FObjective> Objective;
private:
};


UCLASS()
class BLASTER_API UPathChapter : public UObject
{
	GENERATED_BODY()


public:
	UPathChapter();
	void ReloadDataTable(FDataTableRowHandle DataHandle);
	void ReloadDataTable();
	void ReloadRowDataTable(FString RowName);

	void SetNoObjectives(int32 NoObjectives);
	void UpdatePathChapter(FPathChapterDetail* CurrentPathChapter);
	void UpdateObjective();

	void FinishedObjective(int32 ObjectiveNo);
	void ResetObjective();
	void ShowItemNotificationScreen(bool bVisibility);
	void ShowTriggerNotificationScreen(bool bVisibility, TArray<ATrigger*> TargetTrigger);

	void SetCurrentObjectiveLocation(FVector Location);

	UFUNCTION(BlueprintCallable)
	void FinishedPathChapter();
	//no

	UFUNCTION(BlueprintCallable)
	void GiveReward();
	//no

	void ShowMessage();

	UFUNCTION(BlueprintCallable)
	void SetCharacterAssignedToPathChapter(ABlasterCharacter* Character);
	//no
private:
	UPROPERTY()
	FDataTableRowHandle PathChapterHandle;

	FPathChapterDetail* PathChapterDetail;

	FObjective* CurrentObjective; //we will update this pointer later

	FVector PathChapterGoalLocation;

	bool bEndObjective = false;

	int32 ObjectiveNum = -1;

public:
	bool IsExistObjective();
	bool IsEndObjective();
	FPathChapterDetail* GetPathChapterDetail();
	EClearCondition GetCurrentClearCondition();
	FObjective GetCurrentObjective();
	int32 GetCurrentObjectiveNo();
	int32 GetObjectiveNumRequired();
	FVector GetLatestObjectiveLocation();


};
