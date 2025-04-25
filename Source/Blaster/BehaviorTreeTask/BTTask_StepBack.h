#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_StepBack.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBTTask_StepBack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_StepBack(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	enum class EGoBackState { InRange, OutRange};
	EGoBackState LocationRange = EGoBackState::InRange;
};
