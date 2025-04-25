#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blaster/BlasterTypes/Constant.h"
#include "Blaster/Weapon/Weapon.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:

	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	/**
	* This only apply with Projectile Explosive Weapon (Rocket, Grenade, Special)
	*/
	UPROPERTY(EditAnywhere)
	float ExplosiveDamage = DEFAULT_DAMAGE_OVERALL;
	
	UPROPERTY(EditAnywhere)
	float MinimumDamage = DEFAULT_DAMAGE_OVERALL;


	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;	
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = DEFAULT_DAMAGE_INNER_RADIUS;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = DEFAULT_DAMAGE_OUTER_RADIUS;

	UPROPERTY(EditAnywhere)
	float DamageFalloff = DEFAULT_DAMAGE_FALLOFF;



	//UPROPERTY(EditAnywhere)
	bool bHookProjectile = false;

private:

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	UPROPERTY(EditAnywhere)
	bool bGrappleWeapon = false;

	class UParticleSystemComponent* TracerComponent;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 2.2f;


public:	
	FORCEINLINE float GetDamageFalloff() const { return DamageFalloff; }
};
