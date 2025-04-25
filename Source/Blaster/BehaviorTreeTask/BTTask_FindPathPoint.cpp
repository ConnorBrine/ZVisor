#include "BTTask_FindPathPoint.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/Controller.h"
//#include "Blaster/Path/RangePathType.h"

UBTTask_FindPathPoint::UBTTask_FindPathPoint(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase(ObjectInitializer)
{
	NodeName = "Find Path Point";
}

EBTNodeResult::Type UBTTask_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
		{
			int32 Index = Blackboard->GetValueAsInt(TEXT("IdlePathIndex"));

			if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
			{		
				if (Character->GetIdlePath())
				{
					FVector Point = Character->GetIdlePath()->GetIdlePoint(Index);

					FVector GlobalPoint = Character->GetIdlePath()->GetActorTransform().TransformPosition(Point);
					Blackboard->SetValueAsVector(IdlePathVectorKey.SelectedKeyName, GlobalPoint);

					//DrawDebugSphere(GetWorld(), GlobalPoint, 32.f, 12, FColor::Red, true);
					//TODO: Debug with Idle Path

					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}	
			}				
		}
	}
	return EBTNodeResult::Failed;
}
