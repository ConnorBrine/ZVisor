#include "SpawnTrigger.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "Blaster/Spawn/SpawnController.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "TimerManager.h"

ASpawnTrigger::ASpawnTrigger()
{
	
}

void ASpawnTrigger::Tick(float DeltaTime)
{

}

void ASpawnTrigger::GenerateActorSpawn()
{
	GEngine->AddOnScreenDebugMessage(0, 3, FColor::White, FString("Getting Actor Spawn"));
	if (CharacterGameMode->CheckSpawnWeaponItem())
	{
		GEngine->AddOnScreenDebugMessage(0, 3, FColor::White, FString("Passed Spawn Valid"));
		USpawnController::SpawnActor(ActorSpawn->GetDefaultObject(), GetWorld(), GetActorLocation(), GetActorRotation());

		//update later
	}
	

}

void ASpawnTrigger::GenerateCharacterSpawn()
{
	if (CharacterGameMode->CheckSpawnCharacter())
	{
		USpawnController::SpawnCharacter(CharacterSpawn->GetDefaultObject(), GetWorld(), GetActorLocation(), GetActorRotation());
		if (CharacterGameMode)
		{
			CharacterGameMode->IncreaseCharacterNum();
		}
	}
}

void ASpawnTrigger::GenerateScanObjectSpawn()
{
	if (CharacterGameMode)
	{
		CharacterGameMode->SpawnEditorObjectAtLocation(GetActorLocation(), ObjectToSpawn);
		//update later
	}
}

void ASpawnTrigger::BeginPlay()
{
	Super::BeginPlay();
	CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();

	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnTrigger::SpawnActor, TimeToSpawn, bLoopSpawn, FirstDelayTime);
}


void ASpawnTrigger::SpawnActor()
{
	if (bActorSpawn)
	{
		GenerateActorSpawn();
	}
	if (bCharacterSpawn)
	{
		GenerateCharacterSpawn();
	}
	if (bScanObject)
	{
		GenerateScanObjectSpawn();
	}

}