#include "WeaponDetailWidget.h"
#include "Engine/Texture2D.h"

#include "Blueprint/WidgetTree.h"
#include "Templates/SharedPointer.h"

#include "Components/CanvasPanel.h"
#include "Components/PanelWidget.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UWeaponDetailWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//const FString DefaultImage = "/Game/Assets/Images/Weapon_HUD/AK56_HUD.AK56_HUD";
	//UTexture2D* tempTexture = LoadObject<UTexture2D>(this, *DefaultImage);
	/*
	WeaponDetailUI = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("WeaponDetailUI"));
	DetailBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DetailBorder"));
	WeaponImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("WeaponImage"));

	DamageBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("DamageBar"));
	RangeBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("RangeBar"));
	RecoilBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("RecoilBar"));
	RateOfFireBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("RateOfFireBar"));

	SetWeaponNameType("AK56", "AR");
	SetWeaponStat(
		WEAPON_MAX_DAMAGE / 2,
		WEAPON_MAX_RANGE / 2,
		WEAPON_MAX_RECOIL / 2,
		WEAPON_MAX_RATEOFFIRE / 2);

	WidgetTree->RootWidget = WeaponDetailUI;
	auto BaseWidget = Super::RebuildWidget();
	*/
	}

void UWeaponDetailWidget::SetupWeaponDetail()
{

}

void UWeaponDetailWidget::SetWeaponImage(UTexture2D* GivenImage)
{ 
	if (GivenImage == nullptr) return;

	WeaponDetailImage->SetBrushFromTexture(GivenImage, true); //not working right now
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString(WeaponDetailImage->MyImage.IsValid() ? "true" : "false"));
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString(GivenImage->GetName()));
	//!= GivenImage ? "true" : "false"
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString(WeaponImage->GetBrush().GetResourceName().ToString()));
	//WeaponImage = GivenImage;
}

void UWeaponDetailWidget::SetWeaponNameType(FString GivenName, FString GivenType)
{
	WeaponName->SetText(FText::FromString(GivenName));
	WeaponType->SetText(FText::FromString(GivenType));
}

void UWeaponDetailWidget::SetWeaponStat(bool IsMeleeWeapon, float Damage, float Range, float Recoil, float RateOfFire)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, FString::SanitizeFloat(Range));
	float BaseRange = (IsMeleeWeapon ?
		WEAPON_MAX_RANGE_MELEE_STAT : WEAPON_MAX_RANGE_STAT);
	DamageBar->SetPercent(Damage / WEAPON_MAX_DAMAGE_STAT);
	RangeBar->SetPercent(Range / BaseRange);
	RecoilBar->SetPercent(Recoil);
	RateOfFireBar->SetPercent(FMath::Abs(WEAPON_MAX_RATEOFFIRE_STAT - RateOfFire));
}
