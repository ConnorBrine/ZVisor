// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void Fire(const FVector& HitTarget) override;
	void FireTrace(const USkeletalMeshSocket* MuzzleSocket, const FVector& HitTarget, APawn* InstigatorPawn);

	void TransferDamageData();
	void SetWeaponRange();

	float GetVelocityProjectile();
	float GetBaseDamage();

	
protected:

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;
};
