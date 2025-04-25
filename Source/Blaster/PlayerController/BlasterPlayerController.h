#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/PathChapter/PathChapter.h"
#include "BlasterPlayerController.generated.h"

class UImage;
class ABlasterHUD;
class UBarrelWidget;

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDKills(float Kills);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDWeapon(AWeapon* CurrentWeapon);
	/**
	* Barrel
	*/
	void SetHUDBarrel(AWeapon* CurrentWeapon);
	void SetHUDBarrelRemaining(int32 AmmoAmount);
	/**
	* Path Chapter
	*/
	void SetHUDPathChapter(FPathChapterDetail* PathChapterDetail);
	void SetHUDObjective(FPathChapterDetail* PathChapterDetail);
	void SetHUDCaptureTimer(float CurrentCaptureTimer);
	void SetHUDNumRequired(int32 CurrentNumRequire);

	void SetHUDEndObjective();


	void DisableHUDCaptureTimer();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ABlasterHUD* BlasterHUD;
	UBarrelWidget* BarrelWidget;

	UObject* BarrelTemp;

	
	//temp with blueprint
private:

};
