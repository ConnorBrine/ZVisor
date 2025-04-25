#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FinishedInvestigate.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBTTask_FinishedInvestigate : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	explicit UBTTask_FinishedInvestigate(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
