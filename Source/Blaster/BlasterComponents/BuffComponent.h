#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

#define NO_HEALING_REMAIN 0.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();
	friend class ABlasterCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Heal(float HealthAmount, float HealingTime);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
private:
	UPROPERTY()
	class ABlasterCharacter* Character;

	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

public:	
	

		
};
