#include "RapidFollowPath.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/PredictPossibleMove.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/StepBackPath.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/DetectRadius.h"
#include "Blaster/Objects/ActorReceiver.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Engine/ObjectLibrary.h"
#include "Blaster/Objects/ActorReceiver.h"
#include "Blaster/Objects/ScanObjectType.h"

URapidFollowPath::URapidFollowPath()
{
	SetupAISystem();
}

void URapidFollowPath::SetupAISystem()
{
	BaseCharacterLocation = FVector();
	BaseCharacterRotation = FQuat();

	ActorReceiver = CreateDefaultSubobject<UActorReceiver>(FName("ActorReceiver"));
	UE_LOG(LogTemp, Warning, TEXT("CreateDefaultSubobject Successful"));

	SetScanObjectRange(DEFAULT_SCAN_OBJECT_RANGE);
	SetLimitScanObject(LIMIT_OBJECT_IN_RANGE);

	//AIPredictPossibleMove = Cast<UPredictPossibleMove>(CharacterController->GetPerceptionComponent());
	// was AI Sense Prediction

	//AIPredictPossibleMove = Cast<UPredictPossibleMove>(CurrentListenerCharacter);
	//AIDetectRadius;
	//AIStepBackPath;
	//SetupComponent();
	//ListenerCharacter = CurrentListenerCharacter;

	//ActorReceiver = NewObject<UActorReceiver>();
}

void URapidFollowPath::UpdateAISystem(FVector Location, FQuat Rotation)
{
	UpdateLocation(Location);
	UpdateRotation(Rotation);
	
	if (IsDetected())
	{
		UpdateAISystemDetected();
	}


	//AIPredictPossibleMove->UpdateLocation();
	//AIStepBackPath->Update();
	//AIDetectRadius->UpdateDetectRadius();
	// 
}

void URapidFollowPath::UpdateAISystemDetected()
{

	ActorReceiver->UpdateActorReceiver(EScanObjectType::ESOT_Weapon, BaseCharacterLocation);
/* Using this later when character active RFP
	ActorReceiver->UpdateActorReceiver(EScanObjectType::ESOT_AllObjects, CharacterLocation);
	UpdateActorReceiverResult(CharacterLocation);
*/
}

void URapidFollowPath::UpdateLocation(FVector Location)
{
	BaseCharacterLocation = Location;
}

void URapidFollowPath::UpdateRotation(FQuat Rotation)
{
	BaseCharacterRotation = Rotation;
}

void URapidFollowPath::UpdateFriendlyTargetInRange()
{
	//
}

void URapidFollowPath::AddFriendlyTargetInRange(ABlasterCharacter FriendlyTarget)
{
	//FriendlyDataList.Add(FriendlyTarget);
}

void URapidFollowPath::RemoveFriendlyTargetInRange()
{
	//FriendlyDataList.Empty();
}

void URapidFollowPath::NewEnemyTarget(ABlasterCharacter* EnemyTarget)
{
	TargetCharacter = EnemyTarget;
}

void URapidFollowPath::UpdateActorReceiverResult(FVector GivenCharacterLocation)
{
	ActorReceiver->GetObjectsResultData();
}

void URapidFollowPath::SetScanObjectRange(float GivenRange)
{
	ScanObjectRange = GivenRange;

	ActorReceiver->SetScanRange(ScanObjectRange);
}

void URapidFollowPath::SetLimitScanObject(int32 NoOfObject)
{
	UE_LOG(LogTemp, Warning, TEXT("Before Limit: %d"), NoOfObject);
	ActorReceiver->SetLimitObjectsResult(NoOfObject);
}

bool URapidFollowPath::IsDetected()
{
	return bCharacterDetected;
}

void URapidFollowPath::SetDetectedState(bool bDetectedState)
{
	bCharacterDetected = bDetectedState;
}
