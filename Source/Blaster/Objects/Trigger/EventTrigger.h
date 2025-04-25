#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "EventTrigger.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AEventTrigger : public ATrigger
{
	GENERATED_BODY()
public:
	AEventTrigger();
protected:
	virtual void BeginPlay();
	void Event(class AActor* OverlapActor, class AActor* OtherActor);
};
