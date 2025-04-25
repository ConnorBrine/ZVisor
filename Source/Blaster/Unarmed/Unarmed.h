#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unarmed.generated.h"

UCLASS()
class BLASTER_API AUnarmed : public AActor
{
	GENERATED_BODY()
	
public:	
	AUnarmed();

	float Damage = 1.f;

protected:
	virtual void BeginPlay() override;


public:	
	virtual void Tick(float DeltaTime) override;
	void Attack();

};
