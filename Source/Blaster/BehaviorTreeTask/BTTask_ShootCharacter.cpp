#include "BTTask_ShootCharacter.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Weapon/Weapon.h"

UBTTask_ShootCharacter::UBTTask_ShootCharacter(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Shoot Character";
}

EBTNodeResult::Type UBTTask_ShootCharacter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* BlackboardBlackboard = OwnerComp.GetBlackboardComponent())
			{

				Character->AIFire(true);

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;


			}

		}
	}
	return EBTNodeResult::Failed;
}
