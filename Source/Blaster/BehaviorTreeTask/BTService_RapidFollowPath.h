#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/PredictPossibleMove.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "BTService_RapidFollowPath.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBTService_RapidFollowPath : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_RapidFollowPath();
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	FVector GetRapidFollowPathData(ABlasterCharacter* Character, UPredictPossibleMove* PredictPossibleMove);

private:
	bool bActive = false;
	
	UPROPERTY(EditAnywhere)
	float DistanceToDetect = 2000.f;

	UPROPERTY(EditAnywhere)
	float TimeToActive = 10.f;

	UPROPERTY(EditAnywhere)
	float Delay = 10.f;

};
