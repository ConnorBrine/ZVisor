#include "BTTask_FinishedInvestigate.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"

UBTTask_FinishedInvestigate::UBTTask_FinishedInvestigate(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "FinishedInvestigate";
}

EBTNodeResult::Type UBTTask_FinishedInvestigate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
	{
		Blackboard->SetValueAsBool(TEXT("IsPlayerInvestigate"), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
