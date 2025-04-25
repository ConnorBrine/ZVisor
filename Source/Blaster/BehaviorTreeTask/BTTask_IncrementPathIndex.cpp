#include "BTTask_IncrementPathIndex.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "GameFramework/Controller.h"

UBTTask_IncrementPathIndex::UBTTask_IncrementPathIndex(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase(ObjectInitializer)
{
	NodeName = "Increment Path Point";
}

EBTNodeResult::Type UBTTask_IncrementPathIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
			{
				NoOfPoints = Character->GetIdlePath()->Num();
				MaxIndex = NoOfPoints - 1;
				Index = Blackboard->GetValueAsInt(TEXT("IdlePathIndex"));

				if (bBiDirectional)
				{
					if (Index >= MaxIndex && Direction == EDirectionType::Forward)
					{
						Direction = EDirectionType::Reverse;
					}
					if (Index == MinIndex && Direction == EDirectionType::Reverse)
					{
						Direction = EDirectionType::Forward;
					}
				}
				
				Blackboard->SetValueAsInt(GetSelectedBlackboardKey(),
					(Direction == EDirectionType::Forward ? ++Index : --Index) % NoOfPoints);

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;

			}
		}
	}
	return EBTNodeResult::Failed;
}
