#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_TargetCharacterInSight.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBTService_TargetCharacterInSight : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_TargetCharacterInSight();
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
