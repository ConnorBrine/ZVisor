#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "ProjectileHook.generated.h"


#define EXPLODE_TIMER 3.f

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileHook : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileHook();
	void EnablePhysicsConstraint();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* HookCollisionBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HookMesh;

	UPROPERTY(EditAnywhere)
	class UCableComponent* Rope;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Explosive;

	UPROPERTY(EditAnywhere)
	UPhysicsConstraintComponent* PhysicsConstraintComponent;

	UPROPERTY(VisibleAnywhere)
	class UBulletMovementComponent* HookMovementComponent;

	void Explode();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void HookOnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	virtual void OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void ExplosiveOnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	virtual void ProjectileHookOnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:

	TArray<USceneComponent*, TSizedDefaultAllocator<32>> ExplosiveTag;
	TArray<USceneComponent*, TSizedDefaultAllocator<32>> HookTag;

	double GetDistance(FVector GivenA, FVector GivenB) const;
	bool IsHookCloserWithGivenVector(FVector GivenVector);

	bool bHit = false;
	bool bAlreadyAssign = false;
	FVector LatestRopeLocation;
	
public:
	float GetInitialSpeed() const;
	float GetBaseDamage() const;
};
