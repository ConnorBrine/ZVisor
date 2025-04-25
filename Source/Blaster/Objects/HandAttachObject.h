#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "HandAttachObject.generated.h"

UCLASS()
class BLASTER_API AHandAttachObject : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AHandAttachObject();
private:
	UPROPERTY(EditAnywhere, Category = "Object", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox;

	//Recommend using it for ammo and magazine
	UPROPERTY(EditAnywhere, Category = "Object", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh1;

	//Enable this if using empty magazine
	UPROPERTY(EditAnywhere, Category = "Object", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh2;

	UPROPERTY(EditAnywhere, Category = "Object", meta = (AllowPrivateAccess = "true"))
	bool bEnableAdditionMesh = false;

	UPROPERTY(EditAnywhere, Category = "Object", meta = (AllowPrivateAccess = "true"))
	class USoundCue* CollisionSound;

	bool bFirstAppear = false;

protected:

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void DestroyObject();
	void ShowFirstMesh(bool bActiveMesh1);
	void SetSimulation(bool bSimulation);
	void DetachObject();

	UFUNCTION()
	virtual void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
	UFUNCTION()
	virtual void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);
};
