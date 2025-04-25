#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UListView;

UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillsAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PathChapter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PathChapterDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Objective;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* CaptureTimer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RequireTitle;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NumRequire;

	UPROPERTY(meta = (BindWidget))
	UListView* BarrelList;

};
