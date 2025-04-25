#include "BTService_GettingHit.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/Character/BlasterCharacter.h"

UBTService_GettingHit::UBTService_GettingHit()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Getting Hit";
}

void UBTService_GettingHit::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
			{
				bool bGettingHit = Blackboard->GetValueAsBool(TEXT("IsGettingHit"));
			}
		}
	}
}
