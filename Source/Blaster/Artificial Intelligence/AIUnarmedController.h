#pragma once

#include "CoreMinimal.h"
#include "AICharacterController.h"
#include "AIUnarmedController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AAIUnarmedController : public AAICharacterController
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
private:
	class ABlasterCharacter* Character;
};
