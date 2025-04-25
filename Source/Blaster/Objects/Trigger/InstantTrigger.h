#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "Engine/TriggerBox.h"
#include "InstantTrigger.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AInstantTrigger : public ATrigger
{
	GENERATED_BODY()
	
public:
	AInstantTrigger();

	virtual void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor) override;
	virtual void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor) override;

	virtual void SetNotificationScreenVisibility(bool bVisibility) override;

protected:
	virtual void BeginPlay() override;
private:
	bool bOverlapped = false;

};
