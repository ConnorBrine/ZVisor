#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AICharacterController.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "AIShooterController.generated.h"

/**
 * Ignore this class during this still in progressing
 */

UCLASS()
class BLASTER_API AAIShooterController : public AAICharacterController
{
	GENERATED_BODY()
public:
	friend class ABlasterCharacter;
	virtual void Tick(float DeltaTime) override;
	virtual void SightOfPlayer(APawn* PlayerPawn) override;
	virtual void AttachWeapon() override;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_AssaultRifle;

	UPROPERTY(EditAnywhere)
	bool bRandomWeaponAttach = true;

public:

};
