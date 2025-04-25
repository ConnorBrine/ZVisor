#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	AMeleeWeapon();
	virtual void Tick(float DeltaTime) override;


	void LightAttack();
	void HeavyAttack();

	void AttackStart();
	void AttackEnd();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* MeleeDamageCollision;

	UPROPERTY(EditAnywhere, Category="Melee Weapon Damage")
	float DamageMultiply = 1.f;

	bool bAttackStart = false;
	//light: CurrentDamage
	//heavy: CurrentDamage * Interp(StartAnimation -> EndAnimation)
	//                              0.f              DamageMultiply

public:
	float GetWeaponRange();
};
