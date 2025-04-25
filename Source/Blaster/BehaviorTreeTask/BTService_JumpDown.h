#pragma once

#define MAXIMUM_Z_JUMP 500.f
#define MINIMUN_Z_JUMP 45.f

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_JumpDown.generated.h"

UCLASS()
class BLASTER_API UBTService_JumpDown : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_JumpDown();
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
