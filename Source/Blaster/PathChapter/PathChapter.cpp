#include "PathChapter.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/Pickup/PathChapterItemPickup.h"
#include "Blaster/Objects/Trigger/Trigger.h"
#include "Blaster/Objects/Trigger/CaptureTrigger.h"
#include "Blaster/Objects/Trigger/InstantTrigger.h"
#include "Blaster/Spawn/SpawnController.h"
#include "EngineUtils.h"
#include "GameFramework/HUD.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Spawn/SpawnInitializeComponent.h"
#include "Blaster/PathChapter/ClearCondition.h"

UPathChapter::UPathChapter()
{
	//ReloadDataTable();
	//this was only in editor, use by caution 
}

void UPathChapter::ReloadDataTable(FDataTableRowHandle DataHandle)
{
	PathChapterHandle = DataHandle;
}

void UPathChapter::ReloadDataTable()
{
	//PathChapterHandle = USpawnController::GetEditorPathChapterDataTable();
	//PathChapterHandle = 
}

void UPathChapter::ReloadRowDataTable(FString RowName)
{
	do 
	{
		PathChapterDetail = PathChapterHandle.DataTable->FindRow<FPathChapterDetail>(FName(*RowName), "");
	} while (PathChapterDetail == nullptr);
	
	ResetObjective();
}


void UPathChapter::SetNoObjectives(int32 NoObjectives)
{

}

void UPathChapter::UpdatePathChapter(FPathChapterDetail* CurrentPathChapter)
{
	PathChapterDetail = CurrentPathChapter;

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString(PathChapterDetail->Name));
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString(PathChapterDetail->Description));

}

void UPathChapter::UpdateObjective()
{
	if (ObjectiveNum <= -1) return;
	//if (PathChapterDetail->Objective.Num() < ObjectiveNum) return; //limit object number
	PathChapterDetail->Objective[ObjectiveNum].IsComplete = true; //check latest value
	ObjectiveNum++;
}

void UPathChapter::FinishedObjective(int32 ObjectiveNo)
{

}

void UPathChapter::ResetObjective()
{
	//reset all objective into false to prevent some objective 
	//accidently change to true
	for(int32 Index = 0; Index < PathChapterDetail->Objective.Num(); ++Index)
		PathChapterDetail->Objective[Index].IsComplete = false;

	ObjectiveNum = 0;
}

void UPathChapter::ShowItemNotificationScreen(bool bVisibility)
{

	if (ObjectiveNum <= -1) return;

	if (PathChapterDetail->Objective[ObjectiveNum].ItemToCollect)
	{
		FString PathChapterItemPickupName = PathChapterDetail->Objective[ObjectiveNum].ItemToCollect->GetName();
		//fix
		for (TObjectIterator<APathChapterItemPickup> Itr; Itr; ++Itr)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString(Itr->GetName()));
			Itr->SetNotificationScreenVisibility(bVisibility);
		}
	}

	if (PathChapterDetail->Objective[ObjectiveNum].TriggerName != " ")
	{
		FString PathChapterItemPickupName = PathChapterDetail->Objective[ObjectiveNum].TriggerName;
		//fix
		for (TObjectIterator<ATrigger> Itr; Itr; ++Itr)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString(Itr->GetName()));
			//Itr->SetNotificationScreenVisibility(bVisibility);
		}
	}
}

void UPathChapter::ShowTriggerNotificationScreen(bool bVisibility, TArray<ATrigger*> TargetTrigger)
{

	
}

void UPathChapter::SetCurrentObjectiveLocation(FVector Location)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1000, FColor::Blue, FString::SanitizeFloat(Location.X));
	//GEngine->AddOnScreenDebugMessage(-1, 1000, FColor::Blue, FString::SanitizeFloat(Location.Y));
	//GEngine->AddOnScreenDebugMessage(-1, 1000, FColor::Blue, FString::SanitizeFloat(Location.Z));

	PathChapterGoalLocation = Location;
}

void UPathChapter::FinishedPathChapter()
{

}

void UPathChapter::GiveReward()
{
	bEndObjective = true;
}

void UPathChapter::SetCharacterAssignedToPathChapter(ABlasterCharacter* Character)
{

}

bool UPathChapter::IsExistObjective()
{
	return PathChapterDetail->Objective.IsValidIndex(ObjectiveNum);
}

bool UPathChapter::IsEndObjective()
{
	return ObjectiveNum >= PathChapterDetail->Objective.Num();
}

FPathChapterDetail* UPathChapter::GetPathChapterDetail()
{
	return PathChapterDetail;
}

EClearCondition UPathChapter::GetCurrentClearCondition()
{
	EClearCondition CurrentClearCondition = EClearCondition::ECC_Default;
	if (PathChapterDetail)
	{
		if (ObjectiveNum <= -1) return CurrentClearCondition;
		return PathChapterDetail->Objective[ObjectiveNum].ClearType;
	}
	return EClearCondition::ECC_Default;
}

FObjective UPathChapter::GetCurrentObjective()
{
	if (ObjectiveNum <= -1 || !PathChapterDetail)
	{
		FObjective nullObjective{};
		return nullObjective;
	}
	return PathChapterDetail->Objective[ObjectiveNum];
}

int32 UPathChapter::GetCurrentObjectiveNo()
{
	return ObjectiveNum;
}

int32 UPathChapter::GetObjectiveNumRequired()
{
	if (ObjectiveNum <= -1) return 0;
	return PathChapterDetail->Objective[ObjectiveNum].NumRequired;
}

FVector UPathChapter::GetLatestObjectiveLocation()
{
	return PathChapterGoalLocation;
}

