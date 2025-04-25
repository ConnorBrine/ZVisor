#include "BTService_JumpDown.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_JumpDown::UBTService_JumpDown()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Is Player Can Jump Down";
}

void UBTService_JumpDown::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAICharacterController* Controller = Cast<AAICharacterController>(OwnerComp.GetAIOwner()))
	{
		if (ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn()))
		{
			
			if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
			{
				FVector PlayerLocation = Blackboard->GetValueAsVector(TEXT("TargetLocation"));

				double CharacterHeightLocation = Character->GetActorLocation().Z;
				double TargetHeightLocation = PlayerLocation.Z;

				//UE_LOG(LogTemp, Error, TEXT("Jump Here with %f %f = %f %s %s %s"), 
				//	CharacterHeightLocation, TargetHeightLocation, CharacterHeightLocation - TargetHeightLocation,
				//	FMath::RoundToInt(CharacterHeightLocation) != FMath::RoundToInt(TargetHeightLocation) ? TEXT("true") : TEXT("false"),
				//	CharacterHeightLocation - TargetHeightLocation <= MAXIMUM_Z_JUMP ? TEXT("true") : TEXT("false"),
				//	CharacterHeightLocation - TargetHeightLocation > MINIMUN_Z_JUMP ? TEXT("true") : TEXT("false")
				//);

				Blackboard->SetValueAsBool(
					TEXT("IsValidJumpDown"),
					FMath::RoundToInt(CharacterHeightLocation) != FMath::RoundToInt(TargetHeightLocation)
					&& CharacterHeightLocation - TargetHeightLocation <= MAXIMUM_Z_JUMP
					&& CharacterHeightLocation - TargetHeightLocation > MINIMUN_Z_JUMP);
			}
		}
	}
}
