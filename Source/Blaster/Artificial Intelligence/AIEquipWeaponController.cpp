#include "AIEquipWeaponController.h"
#include "Blaster/Character/BlasterCharacter.h"

void AAIEquipWeaponController::BeginPlay()
{
	Super::BeginPlay();
}

void AAIEquipWeaponController::SetupCharacterController()
{
	Super::SetupCharacterController();

	if (bStartedEquippedWeapon)
	{
		FTimerHandle TimerHandle;
		float WeaponAttachDelay = 1.f;

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAICharacterController::AttachWeapon, WeaponAttachDelay, false);
	}

}

