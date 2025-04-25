// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "ActorReceiver.generated.h"

#define DEFAULT_LIMIT_SCAN_OBJECT 100
#define DEFAULT_ACTOR_RECEIVER_RANGE 1000.f

UCLASS()
class BLASTER_API UActorReceiver : public UObject
{
	GENERATED_BODY()

public:
	UActorReceiver();
	
	void SetupActorReceiver();
	void UpdateActorReceiver(EScanObjectType GivenObjectType, FVector GivenCharacterLocation);

protected:

private:
	float MaxScanRange;
	int32 LimitObjectsResult; //TODO: for some reason, this int32 cannot receive anything

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> ObjectsResult;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> RestrictObject;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> IncludeObject;


	EScanObjectType ObjectType;

	void SetCharacterOnly();
	void SetObjectOnly();
	void SetWeaponOnly();
	void SetPickupOnly();
	void SetAll();
	void SetupDefaultObject();

public:	
	void CheckAvailableObjects(FVector GivenCharacterLocation);

	void SetScanRange(float GivenRange);
	void SetLimitObjectsResult(int32 NoOfObjects);

	void ResetObjectsResult();
	
	TArray<AActor*> GetObjectsResultData();
	
};
