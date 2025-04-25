#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Warning, TEXT("Reload Map started..."));
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (false)
	{
		UWorld* World = GetWorld();
		if (World) {
			APlayerController* PlayerController = World->GetFirstPlayerController();
			if (PlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("Begin..."));
				UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
				bUseSeamlessTravel = true;
				World->ServerTravel(FString("/Game/Maps/Lab_Map?listen"));
			}
			
		}
	}
	
	/*if (NumberOfPlayers == 2) {
		UWorld* World = GetWorld();
		if (World) {
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/Lab_Map?listen"));
		} 
	}*/
}
