#pragma once

#include "CoreMinimal.h"
#include "Trigger.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "SpawnTrigger.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ASpawnTrigger : public ATrigger
{
	GENERATED_BODY()
public:
	ASpawnTrigger();

	void SpawnActor();

	virtual void Tick(float DeltaTime) override;

	void GenerateActorSpawn();
	void GenerateCharacterSpawn();
	void GenerateScanObjectSpawn();

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, Category="Spawn")
	float TimeToSpawn = 0.f;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	bool bLoopSpawn = true;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float FirstDelayTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "ActorSpawn")
	bool bActorSpawn = false;

	UPROPERTY(EditAnywhere, Category = "ActorSpawn")
	TSubclassOf<AActor> ActorSpawn;

	UPROPERTY(EditAnywhere, Category = "CharacterSpawn")
	bool bCharacterSpawn = false;

	UPROPERTY(EditAnywhere, Category = "CharacterSpawn")
	TSubclassOf<ABlasterCharacter> CharacterSpawn;

	UPROPERTY(EditAnywhere, Category = "ScanObjectSpawn")
	bool bScanObject = false;

	UPROPERTY(EditAnywhere, Category = "ScanObjectSpawn")
	EScanObjectType ObjectToSpawn;

	UPROPERTY(VisibleAnywhere)
	FTimerHandle SpawnTimerHandle;

	UPROPERTY(VisibleAnywhere)
	ABlasterGameMode* CharacterGameMode;
	
};
