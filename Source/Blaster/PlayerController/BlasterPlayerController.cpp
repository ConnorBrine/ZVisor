#include "BlasterPlayerController.h"
#include "Engine/Texture2D.h"
#include "Blueprint/UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ComponentS/ListView.h"

#include "Blaster/Objects/TempObject.h"
#include "Blaster/Objects/TempUserWidget.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Blaster/HUD/BarrelWidget.h"
#include "Blaster/Spawn/SpawnController.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/Weapon/Weapon.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay(); 
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());

	if (BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BarrelList)
	{
		TSubclassOf<UUserWidget> BaseBarrelWidget = BlasterHUD->CharacterOverlay->BarrelList->GetEntryWidgetClass();
		if (BaseBarrelWidget)
		{
			//UUserWidget* Widget = CreateWidget<UUserWidget>(this, BaseBarrelWidget);
			BarrelWidget = CreateWidget<UBarrelWidget>(this, BaseBarrelWidget);
		}
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay && 
		BlasterHUD->CharacterOverlay->HealthBar && 
		BlasterHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Health));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABlasterPlayerController::SetHUDKills(float Kills)
{
	//UE_LOG(LogTemp, Warning, TEXT("Kill show: %f"), Kills);
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->KillsAmount;
	if (bHUDValid)
	{
		FString KillsText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Kills));
		
		BlasterHUD->CharacterOverlay->KillsAmount->SetText(FText::FromString(KillsText));
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);

		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);

		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDWeapon(AWeapon* CurrentWeapon)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponImage;

	if (bHUDValid)
	{
		FString EquippedName;

		if (CurrentWeapon)
		{
			EquippedName = CurrentWeapon->GetName();
			BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetVisibility(
				CurrentWeapon->IsMeleeWeapon() ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
			BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetVisibility(
				CurrentWeapon->IsMeleeWeapon() ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
			EquippedName = CurrentWeapon->GetName();
			EquippedName.RemoveAt(0, 3);
			EquippedName.RemoveAt(EquippedName.Find(FString("_"), ESearchCase::IgnoreCase), EquippedName.Len() - 1);
		}
		else
		{
			BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetVisibility(ESlateVisibility::Hidden);
			BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetVisibility(ESlateVisibility::Hidden);
			EquippedName = "Hand_HUD";
		}

		ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (BlasterGameMode == nullptr) return;

		for (UTexture2D* Data : BlasterGameMode->ImageData)
		{
			if (Data->GetName().Contains(EquippedName))
			{
				BlasterHUD->CharacterOverlay->WeaponImage->SetBrushFromTexture(Data, true);
				//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString(BlasterHUD->CharacterOverlay->WeaponImage->GetBrush().GetResourceName().ToString()));
			}
		}
	}

}

void ABlasterPlayerController::SetHUDBarrel(AWeapon* CurrentWeapon)
{
	if (CurrentWeapon == nullptr) return;
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BarrelList;

	if (bHUDValid)
	{
		bool bAvailableBarrel = CurrentWeapon && CurrentWeapon->IsMultiBarrel();
		UTexture2D* BarrelImage = CurrentWeapon->GetBarrelImage();

		UListView* BarrelList = BlasterHUD->CharacterOverlay->BarrelList;
		BarrelList->ClearListItems();

		BarrelList->SetVisibility(
			bAvailableBarrel ?
			ESlateVisibility::Visible : ESlateVisibility::Hidden);
		
		if (bAvailableBarrel)
		{

			for (int32 i = 1; 
				i <= CurrentWeapon->GetBarrelAmmo() && 
				BarrelList->GetNumItems() <= CurrentWeapon->GetBarrel();
				++i)
			{
				UTempObject* Object = NewObject<UTempObject>(this, FName(*FString::FromInt(i)));
				BarrelList->AddItem(Object);
			}

			if (BarrelWidget)
			{
				//BarrelWidget->BarrelImage->SetBrushFromTexture(BarrelImage);
			}
		}
		
	}
}

void ABlasterPlayerController::SetHUDBarrelRemaining(int32 AmmoAmount)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BarrelList;

	if (bHUDValid)
	{
		UListView* BarrelList = BlasterHUD->CharacterOverlay->BarrelList;

		if (AmmoAmount > 0)
		{
			for (int i = 1; i <= AmmoAmount; ++i)
			{
				UTempUserWidget* Object = NewObject<UTempUserWidget>(BlasterHUD->GetOuter(), FName(*FString::FromInt(i)));
				BarrelList->AddItem(Object);
			}
			
		}
		else
		{

				int32 Index = BarrelList->GetNumItems() - 1;
				BarrelList->RemoveItem(BarrelList->GetItemAt(Index));

		}
		

	}
}

void ABlasterPlayerController::SetHUDPathChapter(FPathChapterDetail* PathChapterDetail)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay;
	if (bHUDValid)
	{	
		if (BlasterHUD->CharacterOverlay->PathChapter)
		{
			BlasterHUD->CharacterOverlay->PathChapter->SetText(FText::FromString(PathChapterDetail->Name));
		}
		if (BlasterHUD->CharacterOverlay->PathChapterDescription)
		{
			BlasterHUD->CharacterOverlay->PathChapterDescription->SetText(FText::FromString(PathChapterDetail->Description));
		}
		SetHUDObjective(PathChapterDetail);
	}
}

void ABlasterPlayerController::SetHUDObjective(FPathChapterDetail* PathChapterDetail)
{
	if (BlasterHUD->CharacterOverlay->Objective)
	{

		for (FObjective CurrentObjective : PathChapterDetail->Objective)
		{
			if (!CurrentObjective.IsComplete)
			{
				BlasterHUD->CharacterOverlay->Objective->SetText(FText::FromString(CurrentObjective.Description));
				break;
			}
		}

	}
}

void ABlasterPlayerController::SetHUDCaptureTimer(float CurrentCaptureTimer)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CaptureTimer;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->CaptureTimer->SetVisibility(ESlateVisibility::Visible);

		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Black, FString::SanitizeFloat(CurrentCaptureTimer));
		BlasterHUD->CharacterOverlay->CaptureTimer->SetPercent(CurrentCaptureTimer / 100.f);
	}
}

void ABlasterPlayerController::SetHUDNumRequired(int32 CurrentNumRequire)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RequireTitle &&
		BlasterHUD->CharacterOverlay->NumRequire;
	if (bHUDValid)
	{
		if (CurrentNumRequire == 0)
		{
			BlasterHUD->CharacterOverlay->RequireTitle->SetVisibility(ESlateVisibility::Hidden);
			BlasterHUD->CharacterOverlay->NumRequire->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			BlasterHUD->CharacterOverlay->RequireTitle->SetVisibility(ESlateVisibility::Visible);
			BlasterHUD->CharacterOverlay->NumRequire->SetVisibility(ESlateVisibility::Visible);

			FString NumRequireText = FString::Printf(TEXT("%d"), CurrentNumRequire);
			BlasterHUD->CharacterOverlay->NumRequire->SetText(FText::FromString(NumRequireText));
		}
	}
}

void ABlasterPlayerController::SetHUDEndObjective()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->AddCharacterMessage();
	}
}

void ABlasterPlayerController::DisableHUDCaptureTimer()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CaptureTimer;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->CaptureTimer->SetVisibility(ESlateVisibility::Hidden);
	}
	
}


