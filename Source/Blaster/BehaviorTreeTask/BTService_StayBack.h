#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_StayBack.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBTService_StayBack : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_StayBack();
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};
