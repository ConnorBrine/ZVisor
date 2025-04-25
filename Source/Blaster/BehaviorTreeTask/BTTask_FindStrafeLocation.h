#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindStrafeLocation.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBTTask_FindStrafeLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	explicit UBTTask_FindStrafeLocation(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	UEnvQuery* LocationSeekerQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	float Distance = 500.f;

	void LocationSeekerQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	bool IsDistanceGreaterThanX(FVector GivenLocation);
private:

	FEnvQueryRequest LocationSeekerQueryRequest;

	FVector StrafeLocation = FVector::ZeroVector;

};
