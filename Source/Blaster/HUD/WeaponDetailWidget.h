#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Templates/SharedPointer.h"

#include "Blaster/BlasterTypes/Constant.h"

#include "WeaponDetailWidget.generated.h"

USTRUCT(BlueprintType)
struct FWeaponStat
{
	GENERATED_BODY();
public:
	float Damage;
	float Range;
	float Recoil;
	float RateOfFire;

	class UProgressBar* DamageBar;
	UProgressBar* RangeBar;
	UProgressBar* RecoilBar;
	UProgressBar* RateOfFireBar;
};

UCLASS()
class BLASTER_API UWeaponDetailWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UPanelWidget* WeaponDetailUI;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UBorder* DetailBorder;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* WeaponDetailImage;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* WeaponName;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* WeaponType;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* DamageBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* RangeBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* RecoilBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* RateOfFireBar;

	//UPROPERTY(EditAnywhere)
	//TSubClassOf<

	void SetupWeaponDetail();
	void SetWeaponImage(UTexture2D* GivenImage);
	void SetWeaponNameType(FString GivenName, FString GivenType);
	void SetWeaponStat(bool IsMeleeWeapon, float Damage, float Range, float Recoil, float RateOfFire);
private:
	//FWeaponStat WeaponStat;
	//TSharedRef<SWidget> BaseWidget;

	TArray<UTexture2D*> Specialist;
	TArray<UTexture2D*> Attachment;
	//these two array will run later

};
