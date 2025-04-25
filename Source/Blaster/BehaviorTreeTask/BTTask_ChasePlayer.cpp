#include "BTTask_ChasePlayer.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blaster/Character/BlasterCharacter.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase(ObjectInitializer)
{
	NodeName = "Chase Player";
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
		{
			if (Blackboard->GetValueAsBool(TEXT("IsStopMovement"))) return EBTNodeResult::Succeeded;

			if (Blackboard->GetValueAsBool(TEXT("IsStayBack"))) return EBTNodeResult::Succeeded;

			FVector PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TEXT("TargetLocation"));
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, PlayerLocation);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return EBTNodeResult::Succeeded;

		}
	}
	return EBTNodeResult::Failed;
}
