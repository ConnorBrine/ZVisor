#include "MenuSystem.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMenuSystem::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	//UUserWidget::bIsFocusable = true;
	SetIsFocusable(true);
	

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
}

bool UMenuSystem::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (SingleBtn)
	{
		SingleBtn->OnClicked.AddDynamic(this, &ThisClass::SingleBtnClicked);
	}

	if (MultiBtn)
	{
		MultiBtn->OnClicked.AddDynamic(this, &ThisClass::MultiBtnClicked);
	}

	return true;
}

void UMenuSystem::SingleBtnClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Single Button Clicked!"))
		);

		//UGameplayStatics::OpenLevel(this, TEXT("Lab_Map"));
		UWorld* World = GetWorld();
		if (World)
		{
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				
				//PlayerController->SetInputMode(UWidgetBlueprintLibrary::SetInputMode_GameOnly);
				World->ServerTravel(FString("/Game/Maps/Lobby?listen"));
			}
			
		}
	}
}

void UMenuSystem::MultiBtnClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Multi Button Clicked!"))
		);
	}
}
