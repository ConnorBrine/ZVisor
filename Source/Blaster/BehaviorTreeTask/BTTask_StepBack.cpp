#include "BTTask_StepBack.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/Controller.h"

UBTTask_StepBack::UBTTask_StepBack(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Step Back";
}

EBTNodeResult::Type UBTTask_StepBack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
		{
			if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
			{
				bool bInRangePathStats = Character->IsInRangePath();
				FVector StepBackLocation = Character->GetOptimizePath(bInRangePathStats);

				Blackboard->SetValueAsVector(TEXT("IdlePathPoint"), StepBackLocation);

				Character->ResetCurrentRangePath();

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
				
			}
		}
	}

	return EBTNodeResult::Failed;
}

