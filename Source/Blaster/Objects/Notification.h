#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Notification.generated.h"

UCLASS()
class BLASTER_API ANotification : public AActor
{
	GENERATED_BODY()
	
public:	
	ANotification();
	virtual void Tick(float DeltaTime) override;
	void SetNotificationScreenVisibility(bool bVisibility);
private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* NotificationScreen;

protected:
	virtual void BeginPlay() override;

public:	


};
