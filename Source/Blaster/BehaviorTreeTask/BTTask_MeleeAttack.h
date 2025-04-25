#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBTTask_MeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTTask_MeleeAttack(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	bool MontageHasFinished(ABlasterCharacter* Character);
};
