#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h" 
#include "Blaster/PathChapter/PathChapter.h" 
#include "Blaster/GameMode/BlasterGameMode.h"

void ABlasterPlayerState::AddToKills(float KillsAmount)
{


	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		if (Character->IsAICharacter()) return;

		SetScore(GetScore() + KillsAmount);

		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			float Kills = GetScore();
			UE_LOG(LogTemp, Warning, TEXT("Current kills: %f"), Kills);
			Controller->SetHUDKills(Kills);
		}
	}
}

void ABlasterPlayerState::BeginPlay()
{
	//CurrentPathChapter = NewObject<UPathChapter>(this);

	if (CurrentPathChapter)
	{
		ReloadPathChapterDataTable();
	}
	
}

void ABlasterPlayerState::ReloadPathChapterDataTable()
{
	FDataTableRowHandle TableHandle;
	ABlasterGameMode* CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (CharacterGameMode)
	{
		TableHandle = CharacterGameMode->GetTableHandle();

	}
	if (CurrentPathChapter)
	{
		CurrentPathChapter->ReloadDataTable(TableHandle);
		CurrentPathChapter->ReloadRowDataTable("PC1"); //we will using PC1 as demo
		
	}

}

ABlasterPlayerState::ABlasterPlayerState()
{
	CurrentPathChapter = CreateDefaultSubobject<UPathChapter>(TEXT("PathChapter"));
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			float Kills = GetScore();
			UE_LOG(LogTemp, Warning, TEXT("Current kills: %f"), Kills);
			Controller->SetHUDKills(Kills);
		}
		
	}
}

UPathChapter* ABlasterPlayerState::GetPathChapter()
{
	return CurrentPathChapter;
}

