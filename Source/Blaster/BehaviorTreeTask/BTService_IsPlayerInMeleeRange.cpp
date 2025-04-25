#include "BTService_IsPlayerInMeleeRange.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"

UBTService_IsPlayerInMeleeRange::UBTService_IsPlayerInMeleeRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Is Player In Melee Range";
}

void UBTService_IsPlayerInMeleeRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
		{
			bool bMelee = (Character->GetActorLocation() - Controller->GetTargetCharacterLocation()).Size() <= MeleeRange;
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(
				TEXT("PlayerIsInMeleeRange"), bMelee);
		}
	}
	//return OtherActor ? (GetActorLocation() - OtherActor->GetActorLocation()).Size() : 0.f;


}
