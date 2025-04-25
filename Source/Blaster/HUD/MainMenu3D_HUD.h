#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "MainMenu3D_HUD.generated.h"

UCLASS()
class BLASTER_API AMainMenu3D_HUD : public AActor
{
	GENERATED_BODY()
	
public:	
	AMainMenu3D_HUD();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void TransitionTo();

protected:

	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere)
	UWidgetComponent* MenuWidget;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;
};
