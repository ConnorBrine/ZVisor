#include "BTService_TargetCharacterInSight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/Character/BlasterCharacter.h"

UBTService_TargetCharacterInSight::UBTService_TargetCharacterInSight()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Target Character In Sight";
}

void UBTService_TargetCharacterInSight::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
			{
				bool bInSight = Blackboard->GetValueAsBool(TEXT("IsValidTargetCharacterInSight"));
				if (bInSight)
				{

				}
			}
		}
	}
}
