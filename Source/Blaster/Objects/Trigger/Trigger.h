#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Trigger.generated.h"

class UWidgetComponent;

/**
 * 
 */
UCLASS()
class BLASTER_API ATrigger : public ATriggerBox
{
	GENERATED_BODY()
public:
	ATrigger();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* NotificationScreen;

	UFUNCTION()
	virtual void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
	UFUNCTION()
	virtual void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

	virtual void SetNotificationScreenVisibility(bool bVisibility);
	virtual void SetObjectiveLocationVisibility(bool bVisibility);

	void SetNotificationScreenVisibilityTest(bool Test);

	FString GetTriggerName();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FString TriggerName = "null";
private:


};
