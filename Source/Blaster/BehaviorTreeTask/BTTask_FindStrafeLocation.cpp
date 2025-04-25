#include "BTTask_FindStrafeLocation.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "Classes/AIManager.h"

UBTTask_FindStrafeLocation::UBTTask_FindStrafeLocation(FObjectInitializer const& ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = "Find Strafe Location";
}

EBTNodeResult::Type UBTTask_FindStrafeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* BlackboardBlackboard = OwnerComp.GetBlackboardComponent())
			{
				if (LocationSeekerQuery)
				{
					LocationSeekerQueryRequest = FEnvQueryRequest(LocationSeekerQuery, Character);
					LocationSeekerQueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &UBTTask_FindStrafeLocation::LocationSeekerQueryFinished);

					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
			}
			
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_FindStrafeLocation::LocationSeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	int32 Index = 0;
	float CurrentBestScore = 0;
	TArray<FVector> Locations;

	for (FVector& Loc : Locations)
	{
		if (IsDistanceGreaterThanX(Loc) && Result->GetItemScore(Index) > CurrentBestScore)
		{
			StrafeLocation = Loc;
			CurrentBestScore = Result->GetItemScore(Index);
		}
		Index++;
	}
	//Blackboard->SetValueAsVector("MoveToLocation", StrafeLocation);
}

bool UBTTask_FindStrafeLocation::IsDistanceGreaterThanX(FVector GivenLocation)
{
	return false;
}
