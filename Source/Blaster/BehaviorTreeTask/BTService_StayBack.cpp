#include "BTService_StayBack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"

UBTService_StayBack::UBTService_StayBack()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Stay Back";
}

void UBTService_StayBack::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
			{
				if (Blackboard->GetValueAsBool(TEXT("IsStopMovement"))) return;
				
				if (Blackboard->GetValueAsBool(TEXT("IsStayBack")))
				{
					////GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, FString("stay back active"));
					//int32 MoveRight = FMath::RandRange(-1, 1);
					//Character->SetAIMovement(-1.f, MoveRight);
				}	
			}
		}
	}
}
