#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BulletMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBulletMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()
protected:
	virtual EHandleBlockingHitResult HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining) override;
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;
};
