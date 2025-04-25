#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_GettingHit.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBTService_GettingHit : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_GettingHit();
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
