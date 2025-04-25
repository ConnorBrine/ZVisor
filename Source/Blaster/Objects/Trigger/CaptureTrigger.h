// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "CaptureTrigger.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ACaptureTrigger : public ATrigger
{
	GENERATED_BODY()
public:
	ACaptureTrigger();
	virtual void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor) override;
	virtual void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor) override;

	virtual void SetNotificationScreenVisibility(bool bVisibility) override;
protected:
	virtual void BeginPlay();
private:
	bool bOverlapped = false;
};
