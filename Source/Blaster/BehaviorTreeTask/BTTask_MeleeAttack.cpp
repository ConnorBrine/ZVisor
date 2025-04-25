#include "BTTask_MeleeAttack.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine/LatentActionManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Blaster/BlasterComponents/CombatComponent.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "Melee Attack";
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bool OutOfRange = !OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
	if (OutOfRange)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (Controller)
	{
		ABlasterCharacter* Character = Cast<ABlasterCharacter>(Controller->GetPawn());
		if (Character)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString("melee BT"));
			Character->Melee();
		}
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Type();
	
}

bool UBTTask_MeleeAttack::MontageHasFinished(ABlasterCharacter* Character)
{
	return false;
}
