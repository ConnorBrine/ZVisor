#include "BTTask_FindPlayerLocation.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "EngineUtils.h"


UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase(ObjectInitializer)
{
	NodeName = "Find Player Location";
}

EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (Controller->GetTargetCharacter())
		{
			FVector PlayerLocation = Controller->GetTargetCharacterLocation();
			if (SearchRadius)
			{
				if (UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
				{
					FNavLocation Loc;
					bool Test = NavSys->GetRandomPointInNavigableRadius(PlayerLocation, SearchRadius, Loc);
					//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString("true"));
					if (Test)
					{
						OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), PlayerLocation);
						FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
						return EBTNodeResult::Succeeded;
					}
				}
			}
		}
	}
	return EBTNodeResult::Failed;
	//for (TActorIterator<ABlasterCharacter>ActorItr(GetWorld()); ActorItr; ++ActorItr)
	//{
	//	FVector PlayerLocation = ActorItr->GetActorLocation();
	//	if (SearchRadius)
	//	{
	//		if (UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
	//		{
	//			FNavLocation Loc;
	//			bool Test = NavSys->GetRandomPointInNavigableRadius(PlayerLocation, SearchRadius, Loc);
	//			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString("true"));
	//			if (Test)
	//			{
	//				//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString("true"));
	//				OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), PlayerLocation);

	//			}
	//			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString("false"));
	//			
	//		}
	//	}
	//}
	/*FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;*/

	/*if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		
	}
	*/
}

