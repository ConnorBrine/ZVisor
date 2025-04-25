#include "BTTask_LocateNextPathPoint.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/Controller.h"

UBTTask_LocateNextPathPoint::UBTTask_LocateNextPathPoint(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Locate Next Path Point";
}

EBTNodeResult::Type UBTTask_LocateNextPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
		{
			if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
			{		
				int32 NextIndex = Character->GetNextIdlePathIndex();

				Blackboard->SetValueAsInt(TEXT("IdlePathIndex"), NextIndex);
				Blackboard->SetValueAsBool(TEXT("IsIdleState"), true);

				UE_LOG(LogTemp, Warning, TEXT("Next index in locate: %d"), NextIndex);

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;				
			}
		}
	}

	return EBTNodeResult::Failed;
}
