#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()
public:
	AProjectileBullet();

	float GetInitialSpeed() const;

protected:

	UPROPERTY(VisibleAnywhere)
	class UBulletMovementComponent* ProjectileBulletMovementComponent;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	float GetProjectileHitbox(class ABlasterCharacter* HitCharacter, class ABlasterCharacter* CauserCharacter, FHitResult HitBone);
};
