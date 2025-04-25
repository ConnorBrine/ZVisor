#include "BTTask_FindRandomLocation.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Blaster/Character/BlasterCharacter.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase(ObjectInitializer)
{
	NodeName = "Find Random Location in NavMesh";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (APawn* CharacterPawn = Controller->GetPawn())
		{
			FVector Origin = CharacterPawn->GetActorLocation();

			if (UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation Loc;
				Loc.Location;				
				if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, Loc))
				{
					//UE_LOG(LogTemp, Warning, TEXT("This Random will be run!"));
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), Loc.Location);
					/*UE_LOG(LogTemp, Warning, TEXT("Coordinate: x = %f | y = %f | z = %f"),
						Loc.Location.X,
						Loc.Location.Y,
						Loc.Location.Z);*/
				}
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
