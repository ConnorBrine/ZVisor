#pragma once

#include "CoreMinimal.h"
#include "AI/NavigationSystemBase.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "DetectRadius.h"
#include "Blaster/Objects/ActorReceiver.h"
//#include "UObject/NoExportTypes.h"
#include "RapidFollowPath.generated.h"

#define LIMIT_OBJECT_IN_RANGE 100
#define DEFAULT_SCAN_OBJECT_RANGE 1000.f


/**
 * 
 */
UCLASS()
class BLASTER_API URapidFollowPath : public UObject
{
	GENERATED_BODY()
	
private:
	FVector BaseCharacterLocation;
	FQuat BaseCharacterRotation;

	TArray<ABlasterCharacter>FriendlyDataList;
	TArray<AActor*>ObjectInRange;

	ABlasterCharacter* TargetCharacter;

	UPROPERTY(EditAnywhere)
	float ScanObjectRange;
	UWorld* CurrentWorld;

	//AI Feature System
	class UPredictPossibleMove* AIPredictPossibleMove;
	class UDetectRadius* AIDetectRadius;
	class UAStepBackPath* AIStepBackPath;

	UActorReceiver* ActorReceiver;

	bool bCharacterDetected;

protected:

public:
	URapidFollowPath();
	void SetupAISystem();
	void UpdateAISystem(FVector Location, FQuat Rotation);
	void UpdateAISystemDetected();
	

	void UpdateLocation(FVector Location);
	void UpdateRotation(FQuat Rotation);
	void UpdateFriendlyTargetInRange();

	void AddFriendlyTargetInRange(ABlasterCharacter FriendlyTarget);
	void RemoveFriendlyTargetInRange();
	void NewEnemyTarget(ABlasterCharacter* EnemyTarget);

	void UpdateActorReceiverResult(FVector GivenCharacterLocation);

	void SetScanObjectRange(float GivenRange);
	void SetLimitScanObject(int32 NoOfObject);
	void SetDetectedState(bool bDetectedState);

	bool IsDetected();

};
