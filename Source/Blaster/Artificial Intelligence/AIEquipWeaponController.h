#pragma once

#include "CoreMinimal.h"
#include "AICharacterController.h"
#include "AIEquipWeaponController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AAIEquipWeaponController : public AAICharacterController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupCharacterController() override;
private:

	UPROPERTY(EditAnywhere)
	bool bStartedEquippedWeapon = true;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_AssaultRifle;

	UPROPERTY(EditAnywhere)
	bool bRandomWeaponAttach = true;
};
