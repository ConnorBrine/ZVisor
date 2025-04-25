#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ShootCharacter.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBTTask_ShootCharacter : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_ShootCharacter(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
