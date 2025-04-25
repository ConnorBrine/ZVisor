#include "ProjectileWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"

#include "Projectile.h"
#include "ProjectileBullet.h"
#include "ProjectileRocket.h"
#include "ProjectileGrenade.h"
#include "ProjectileHook.h"
#include "RocketMovementComponent.h"
#include "Blaster/BlasterTypes/Constant.h"

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	if (!HasAuthority()) return;

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("Muzzle"));
	//except grenade launcher and rocket launcher
	if (MuzzleSocket)
	{
		FireTrace(MuzzleSocket, HitTarget, InstigatorPawn);
	}
	else
	{
		//TODO: This code wont run, stay there until find the solution
		const USkeletalMeshSocket* RocketSocket = GetWeaponMesh()->GetSocketByName(FName("Ammo1"));
		FireTrace(RocketSocket, HitTarget, InstigatorPawn);

		//use with double barrel and multi rocket barrel
		//RocketSocket = GetWeaponMesh()->GetSocketByName(FName("Ammo2"));
		//FireTrace(MuzzleSocket, HitTarget, InstigatorPawn);
		//RocketSocket = GetWeaponMesh()->GetSocketByName(FName("Ammo3"));
		//FireTrace(MuzzleSocket, HitTarget, InstigatorPawn);
		//RocketSocket = GetWeaponMesh()->GetSocketByName(FName("Ammo4"));
		//FireTrace(MuzzleSocket, HitTarget, InstigatorPawn);
	}
}

void AProjectileWeapon::FireTrace(const USkeletalMeshSocket* MuzzleSocket, const FVector& HitTarget, APawn* InstigatorPawn)
{
	FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
	// From Muzzle flash socket to hit location from TraceUnderCrosshairs
	FVector ToTarget = HitTarget - SocketTransform.GetLocation();
	FRotator TargetRotation = ToTarget.Rotation();
	if (ProjectileClass && InstigatorPawn)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(
				ProjectileClass,
				SocketTransform.GetLocation(),
				TargetRotation,
				SpawnParams
			);
		}
	}
}

void AProjectileWeapon::TransferDamageData()
{

}

float AProjectileWeapon::GetVelocityProjectile()
{
	
	if (
		AProjectileBullet* Bullet = Cast<AProjectileBullet>(ProjectileClass->GetDefaultObject())
		) return Bullet->GetInitialSpeed();
	
	
	if (
		AProjectileGrenade* Grenade = Cast<AProjectileGrenade>(ProjectileClass->GetDefaultObject())
		) return Grenade->GetInitialSpeed();
	
	
	if (
		AProjectileRocket* Rocket = Cast<AProjectileRocket>(ProjectileClass->GetDefaultObject())
		) return Rocket->GetInitialSpeed();
	

	if (
		AProjectileHook* Hook = Cast<AProjectileHook>(ProjectileClass->GetDefaultObject())
		) return Hook->GetInitialSpeed();
	
	return 0.0f;
}

float AProjectileWeapon::GetBaseDamage()
{
	
	if (
		AProjectileBullet* Bullet = Cast<AProjectileBullet>(ProjectileClass->GetDefaultObject())
		) return -1.f;
	//as using damage, if return -1 will automatic getting base body damage

	if (
		AProjectileGrenade* Grenade = Cast<AProjectileGrenade>(ProjectileClass->GetDefaultObject())
		) return Grenade->GetBaseDamage();


	if (
		AProjectileRocket* Rocket = Cast<AProjectileRocket>(ProjectileClass->GetDefaultObject())
		) return Rocket->GetBaseDamage();


	if (
		AProjectileHook* Hook = Cast<AProjectileHook>(ProjectileClass->GetDefaultObject())
		) return Hook->GetBaseDamage();

	return 0.0f;
}

void AProjectileWeapon::SetWeaponRange()
{
	//as we know:
	//R = v * t = v * sqrt((2 * h) / g)
	//R: Range, 
	//v (m/s): velocity (getting by Initial Speed) 100 cm/s -> 1m/s
	//h: height (which define as the height of character)
	//g: gravity (constant 9.81)
	//GetVelocityProjectile() is return as cm/s by Inital Speed Default

	//meter range
	WeaponRange = (GetVelocityProjectile() / 100) * FMath::Sqrt((2 * DEFAULT_HEIGHT_PROJECTILE_HORIZON) / DEFAUNT_GRAVITY); 

	//this will convert meter to kilometer range
	WeaponRange /= 1000;
}
