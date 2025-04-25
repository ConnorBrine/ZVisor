#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BarrelWidget.generated.h"

/**
 * 
 */
class UImage;

UCLASS()
class BLASTER_API UBarrelWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* BarrelImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* EmptyImage;
};
