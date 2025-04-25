#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blaster/BlasterTypes/Constant.h"
#include "OverheadWidget.generated.h"

class UTextBlock;
class UProgressBar;

/**
 *
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SuspectBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayText;

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);


	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHealth(float Health, float MaxHealth);

	UFUNCTION(BlueprintCallable)
	void ShowHealthBar(bool bShow);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerSuspect(float SuspectNum);

	UFUNCTION(BlueprintCallable)
	void ShowSuspectBar(bool bShow);

protected:
	virtual void NativeDestruct() override;		
};
