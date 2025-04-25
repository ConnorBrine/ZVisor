#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Blaster/BlasterTypes/Constant.h"

#include "Pickup.generated.h"

#define PICKUP_SPHERE_RADIUS 150.f


UCLASS()
class BLASTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	/**
	* Enable / Disable Custom Depth
	*/
	void EnableCustomDepth(bool bEnable);
private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;
	
	UPROPERTY(EditAnywhere)
	class USoundCue* PickupSound; 

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* NotificationScreen;

	bool bCollisionObject = false;

	
public:	

	virtual void SetNotificationScreenVisibility(bool bVisibility);
};
