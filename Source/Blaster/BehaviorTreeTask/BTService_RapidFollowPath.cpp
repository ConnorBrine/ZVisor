#include "BTService_RapidFollowPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/RapidFollowPath.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/PredictPossibleMove.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "TimerManager.h"

UBTService_RapidFollowPath::UBTService_RapidFollowPath()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Rapid Follow Path";
}

void UBTService_RapidFollowPath::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
			{
				bool IsInPPM = Blackboard->GetValueAsBool(TEXT("IsActivePPM"));
				if (IsInPPM)
				{
					if (UPredictPossibleMove* PredictPossibleMove = Controller->GetPredictPossibleMove())
					{
						FVector Result = GetRapidFollowPathData(Character, PredictPossibleMove);
						
						if (Result != FVector())
						{
							Blackboard->SetValueAsVector(TEXT("TargetLocation"), Result);
						}
						PredictPossibleMove->Clear();
						Blackboard->SetValueAsBool(TEXT("IsActivePPM"), false);
					}
				}
			}
			
		}
	}
}

FVector UBTService_RapidFollowPath::GetRapidFollowPathData(ABlasterCharacter* Character, UPredictPossibleMove* PredictPossibleMove)
{
	int32 Count = 0;

	for (TObjectIterator<AActor> Itr; Itr; ++Itr)
	{
		float Distance = Character->GetDistanceTo(*Itr);

		FVector ShortestObjectLocation;
		float ShortestDistance = Itr->ActorGetDistanceToCollision(Character->GetActorLocation(), ECollisionChannel::ECC_Visibility, ShortestObjectLocation);

		if (Distance > 0 && Distance <= DistanceToDetect
			|| ShortestDistance > 0 && ShortestDistance <= DistanceToDetect)
		{
			PredictPossibleMove->PreloadObject(*Itr, Character->GetActorLocation(), ShortestObjectLocation, Character->GetActorRotation(), Itr->GetActorRotation());
			Count = 1;
		}

	}
	PredictPossibleMove->AutoPredictPossibleMoveFromLatestData();
	return PredictPossibleMove->GetFinalMove();

}