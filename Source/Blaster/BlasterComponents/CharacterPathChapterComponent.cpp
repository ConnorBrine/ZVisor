#include "CharacterPathChapterComponent.h"
#include "Blaster/PathChapter/PathChapter.h"
#include "Blaster/Objects/Notification.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Spawn/SpawnController.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

UCharacterPathChapterComponent::UCharacterPathChapterComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}



void UCharacterPathChapterComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character && Character->IsAICharacter() == false)
	{
		GetOwner()->GetWorldTimerManager().SetTimer(ObjectiveHandle, this, &UCharacterPathChapterComponent::UpdateObjective, 2.f, false, 2.f);
	}
}


void UCharacterPathChapterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PathChapterInit();
	CaptureTimerInit();
}

void UCharacterPathChapterComponent::PathChapterInit()
{
	//PathChapter = PathChapter == nullptr ? BlasterPlayerState->GetPathChapter() : PathChapter;
}

void UCharacterPathChapterComponent::CaptureTimerInit()
{
	if (BaseIncreaseCaptureTimer == 0.f) return; //check if capture timer is active
	if (Character->IsAICharacter() || bOverlappedCapture == false) return;
	IncreaseCaptureTimer();
	UpdateCaptureTimer();

}

void UCharacterPathChapterComponent::ActivePathChapter()
{
	if (PathChapter == nullptr)
	{
		if (BlasterPlayerState && BlasterPlayerState->GetPathChapter())
		{
			PathChapter = BlasterPlayerState->GetPathChapter();
		}
	}
	bActivePathChapter = true;

	Character->UpdateHUDPathChapter();
}

void UCharacterPathChapterComponent::LoadObjective()
{
	if (PathChapter->GetPathChapterDetail())
	{
		CurrentObjectiveNum = PathChapter->GetCurrentObjectiveNo();
		if (CurrentObjectiveNum != -1)
		{
			FObjective CurrentObjective = PathChapter->GetCurrentObjective();

			CurrentClearCondition = PathChapter->GetCurrentClearCondition();
			BaseIncreaseCaptureTimer = CurrentObjective.CaptureTimer;
			NoObjectiveRemain = CurrentObjective.NumRequired;

			switch (CurrentClearCondition)
			{
			case EClearCondition::ECC_Collect:
			{
				LoadPathChapterItem(
					CurrentObjective.ItemToCollect,
					CurrentObjective.NumRequired,
					CurrentObjective.bSamePlace,
					CurrentObjective.TriggerName,
					PathChapter->GetLatestObjectiveLocation());
				break;
			}

			case EClearCondition::ECC_Travel:
			case EClearCondition::ECC_Capture:
			{
				LoadTriggers(
					CurrentObjective.TriggerName,
					CurrentObjective.bSamePlace,
					CurrentObjective.NumRequired
				);
				break;
			}

			}
		}
	}
	//update objective data
}

void UCharacterPathChapterComponent::LoadPathChapterItem(TSubclassOf<APathChapterItemPickup> Item, int32 NumItemInMap, bool bSameTriggerPlace, FString TriggerName, FVector LocationForSpawn)
{
	ABlasterGameMode* CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	UObject* ItemToCollect = Item->GetDefaultObject();

	//object if NumItemInMap is 1, return here 
	//using for spawn same trigger location / random location
	if (NumItemInMap == 1)
	{
		FVector SpawnLocation = bSameTriggerPlace ?
			LocationForSpawn : CharacterGameMode->GetRandomLocationAtFloorMap();

		SetPathChapterLocation(SpawnLocation);

		USpawnController::SpawnActor(ItemToCollect, GetWorld(), SpawnLocation, CharacterGameMode->GetRandomRotation());
	}

	//NumItemInMap more than 1
	else
		for (int NumToSpawn = 1; NumToSpawn <= NumItemInMap + 14; ++NumToSpawn)
			//TODO: will find another way
		{
			USpawnController::SpawnActor(
				ItemToCollect,
				GetWorld(),
				CharacterGameMode->GetRandomLocationAtFloorMap(),
				CharacterGameMode->GetRandomRotation());
		}
}

void UCharacterPathChapterComponent::LoadTriggers(FString TriggerName, bool bSamePlace, int32 NumOfTrigger)
{
	TArray<ATrigger*> TempTriggersName;

	//load trigger with same trigger name
	for (TActorIterator<ATrigger> Trigger(GetWorld()); Trigger; ++Trigger)
	{
		if (Trigger->GetTriggerName().Contains(TriggerName))
		{
			++TriggersPoint;
			TempTriggersName.Add(*Trigger);
		}
	}

	//check valid if objective give too much than expected in map triggers
	if (NumOfTrigger > TriggersPoint) return;

	//this condition is check is needed for using latest data
	//if false refresh everything to generate new array value
	if (bSamePlace == false)
	{
		//bSamePlace is return false, we do not need latest data
		TriggersPointArray.Empty();

		//Random Trigger number and storage data 
		for (int32 i = 1; i <= NumOfTrigger; ++i)
		{
			int32 LocationNum = FMath::RandRange(1, TriggersPoint);
			TriggersPointArray.Add(LocationNum);
		}
	}

	//load all number above, then check if valid 
	for (int32 No = 0; No < TriggersPointArray.Num(); ++No)
	{
		FString TriggerNameNumber = TriggerName + FString::FromInt(TriggersPointArray[No]);

		for (ATrigger* FindTrigger : TempTriggersName)
		{
			//check valid trigger name
			if (FindTrigger->GetTriggerName().Contains(TriggerNameNumber))
			{
				//this code somehow will take only first index
				//we will update that can be use with
				//multiple trigger
				ATrigger* CurrentTrigger = FindTrigger;
				Triggers.Add(CurrentTrigger);
				TriggersName.Add(CurrentTrigger->GetTriggerName());

				SetPathChapterLocation(CurrentTrigger->GetActorLocation());
				
				//this below with ObjectiveLocation
				ObjectiveLocationVector = CurrentTrigger->GetActorLocation();
				Character->UpdateObjectiveLocation();

				//ObjectiveLocation->SetActorLocation(CurrentTrigger->GetActorLocation());
				ObjectiveLocation->SetHidden(false);

				/*GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString(CurrentTrigger->GetTriggerName()));*/
				break;
			}
		}
	}
}

void UCharacterPathChapterComponent::TriggerPathChapter(FString CollisionName)
{
	if (Character->IsAICharacter()) return;

	if (CurrentClearCondition == EClearCondition::ECC_Default) return;

	if (BlasterPlayerState &&
		BlasterPlayerState->GetPathChapter())
	{

		//FString ObjectiveData = CurrentObjective.TriggerName;

		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("Data: " + ObjectiveData));
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("Data 2: " + CollisionName));

		if (CurrentClearCondition == EClearCondition::ECC_Travel)
		{
			if (TriggersName.Contains(CollisionName))
			{
				UpdateObjective();
			}
			return;
		}

		if (CurrentClearCondition == EClearCondition::ECC_Capture)
		{
			//TriggersName.Contains() TODO::will Multi trigger

			if (TriggersName.Contains(CollisionName))
			{ //inside the capture trigger
				LatestCaptureTrigger = CollisionName;
				bOverlappedCapture = true;
				return;
			}

			//outside the capture trigger
			bOverlappedCapture = false;
			Character->DisableHUDCaptureTimer();

			if (!TriggersName.Contains(LatestCaptureTrigger))
			{ //wrong capture trigger will reset
				CharacterCaptureTimer = 0.f;
			}
			return;

		}
	}
}



void UCharacterPathChapterComponent::UpdateCaptureTimer()
{
	if (CharacterCaptureTimer >= 100.f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, FString("max progress"));

		bOverlappedCapture = false;
		Character->DisableHUDCaptureTimer();

		UpdateObjective();

		return;
	}
	Character->UpdateHUDCaptureTimer();
}

void UCharacterPathChapterComponent::IncreaseCaptureTimer()
{
	CharacterCaptureTimer = FMath::Clamp(CharacterCaptureTimer + BaseIncreaseCaptureTimer, 0.f, 100.f);
}

void UCharacterPathChapterComponent::ResetCaptureTimer()
{
	CharacterCaptureTimer = 0.f;
	BaseIncreaseCaptureTimer = 0.f;
}

void UCharacterPathChapterComponent::ClearTriggersData()
{
	Triggers.Empty();
	TriggersName.Empty();
}

void UCharacterPathChapterComponent::SetPathChapterLocation(FVector LocationForSpawn)
{
	if (PathChapter) 
	{
		PathChapter->SetCurrentObjectiveLocation(LocationForSpawn);
	}
		
}

void UCharacterPathChapterComponent::UpdateObjective()
{
	if (Character == nullptr) return;
	if (Character->IsAICharacter()) return;
	RemoveOldObjective();

	GetOwner()->GetWorldTimerManager().SetTimer(ObjectiveHandle, this, &UCharacterPathChapterComponent::SetupNewObjective, DEFAULT_TIMER_OBJECTIVE_UPDATE, false);

	GetOwner()->GetWorldTimerManager().SetTimer(ObjectiveHUDHandle, this, &UCharacterPathChapterComponent::UpdateCharacterPathChapterHUD, DEFAULT_TIMER_OBJECTIVE_UPDATE_HUD, false);
	//HUD
}

void UCharacterPathChapterComponent::UpdateCharacterPathChapterHUD()
{
	Character->UpdateHUDObjective();
	Character->UpdateHUDNumRequired();
}

void UCharacterPathChapterComponent::RemoveOldObjective()
{
	if (Character == nullptr) return;
	if (Character->IsAICharacter()) return;

	//Set Old Notification Screen Trigger
	ShowItemNotificationScreen(false);

	//reset notification 
	Notification.Empty();

	//reset captureTimer
	ResetCaptureTimer();

	//reset trigger (as some variable will not reset for checking is same location)
	TriggersPoint = 0;

	//Event for disable objective location
	Character->DisableObjectLocation();


	switch (CurrentClearCondition)
	{
	case EClearCondition::ECC_Default: return;

	case EClearCondition::ECC_Travel:
	case EClearCondition::ECC_Capture:
	{
		//prevent player collide trigger, then remove all in trigger data
		for (ATrigger* Trigger : Triggers)
		{
			Trigger->Destroy();
			//Trigger->SetHidden(true);
		}
		ObjectiveLocation->SetHidden(true);
		ClearTriggersData();
		return;
	}

	case EClearCondition::ECC_Collect:
	{
		//UObject* ItemToCollect = BlasterPlayerState->GetPathChapter()->GetCurrentObjective().ItemToCollect->GetDefaultObject();

		for (TActorIterator<APathChapterItemPickup>Pickup(GetWorld()); Pickup; ++Pickup)
		{
			Pickup->Destroy(); //destroy remain
			//This method will fix as all path chapter item accidently destroy
		}
		return;
	}
	}
	//todo: Unhide some trigger


}

void UCharacterPathChapterComponent::SetupNewObjective()
{
	if (Character == nullptr) return;
	if (Character->IsAICharacter() && bObjectiveCheck) return;

	PathChapter = PathChapter == nullptr ? BlasterPlayerState->GetPathChapter() : PathChapter; //still check if PathChapter pointer missing 

	//move to new objective
	if (bActivePathChapter) PathChapter->UpdateObjective();
	else //first time active path chapter
	{
		ObjectiveLocation = GetWorld()->SpawnActor<AActor>(Character->ObjectiveLocation, Character->GetActorLocation(), FRotator());
		ObjectiveLocation->SetHidden(true);

		ActivePathChapter();
		GetOwner()->GetWorldTimerManager().SetTimer(LoadObjectFirstTimerHandle, this, &UCharacterPathChapterComponent::LoadObjective, DEFAULT_TIMER_OBJECTIVE_UPDATE, false);
		return;
	}

	//check is the end of objective
	if (PathChapter->IsEndObjective())
	{
		Character->ShowHUDEndObjective();

		//event for BP for pause the game
		Character->EndObjective(); 
		
		return;
	}

	LoadObjective();

	//Set New Notification Screen Item (update this later)
	//ShowItemNotificationScreen(true);

	//Set New Notification Screen Trigger
	//PathChapter->ShowTriggerNotificationScreen(true, Triggers);

	//set num required (optional)
	NoObjectiveRemain = PathChapter->GetObjectiveNumRequired();

}

void UCharacterPathChapterComponent::ShowItemNotificationScreen(bool bVisibility)
{
	if (Character == nullptr) return;

	//this function will update later
	if (Character->IsAICharacter() && bObjectiveCheck) return;
	if (bActivePathChapter == false || Triggers.IsEmpty()) return;

	ABlasterGameMode* CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();

	if (CurrentObjectiveNum == -1) return;
	bInCollectObjective = true;


	UE_LOG(LogTemp, Warning, TEXT("SIZE Trigger: %d"), Triggers.Num());
	int32 TriggerIndex = 0;

	// ATrigger * Trigger : TriggersName
	for (int32 Item = 0; Item < TriggersPointArray.Num(); ++Item)
	{
		if (bVisibility)
		{
			ANotification* TempNotification = USpawnController::SpawnNotification(CharacterGameMode->GetNotificationData(), GetWorld(), Triggers[Item]->GetActorLocation(), Triggers[Item]->GetActorRotation());
			if (TempNotification)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString("Trigger Notification success place with name:") + FString(*TempNotification->GetName()));
			}
			Notification.Add(TempNotification);
		}
		else
		{
			if (!Notification.IsEmpty() && Notification[Item])
			{
				Notification[Item]->Destroy();
			}

		}
	}

}

void UCharacterPathChapterComponent::PickUpObjectiveItem()
{
	if (Character == nullptr) return;
	if (Character->IsAICharacter() && bObjectiveCheck) return;

	bInCollectObjective = true;

	if (CurrentClearCondition == EClearCondition::ECC_Default) return;

	if (CurrentClearCondition == EClearCondition::ECC_Collect ||
		IsInKillCollectState)
	{
		bInCollectObjective = true;
		DecreasePickUpRequireObjective();
		//NoObjectiveRemain--;
		if (NoObjectiveRemain <= 0)
		{
			UpdateObjective();
			return;
		}
		Character->UpdateHUDNumRequired();
	}
	bObjectiveCheck = false;
}

void UCharacterPathChapterComponent::AddToKillWithObjective(AActor* DamageCharacter)
{
	if (Character == nullptr) return;
	if (Character->IsAICharacter() && bObjectiveCheck) return;
	if (DamageCharacter)
	{
		bObjectiveCheck = true;

		if (CurrentClearCondition == EClearCondition::ECC_Default) return;

		if (CurrentClearCondition == EClearCondition::ECC_KillCollect)
		{
			NoObjectiveRemain--;
			if (NoObjectiveRemain == 0)
			{

				GetWorld()->SpawnActor<AActor>(
					PathChapter->GetCurrentObjective().ItemToCollect,
					DamageCharacter->GetActorLocation(),
					DamageCharacter->GetActorRotation());
				NoObjectiveRemain = 1; //item need to be collect
				IsInKillCollectState = true; //enable to run with collect state
			}
			Character->UpdateHUDNumRequired();
		}

	}
	bObjectiveCheck = false;
}

void UCharacterPathChapterComponent::DecreasePickUpRequireObjective()
{
	NoObjectiveRemain--;
}
