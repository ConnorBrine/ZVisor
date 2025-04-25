#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PathChapterItemPickup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API APathChapterItemPickup : public APickup
{
	GENERATED_BODY()
public:
	virtual void SetNotificationScreenVisibility(bool bVisibility) override;
protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
private:

};
