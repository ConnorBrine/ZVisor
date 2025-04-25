#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UMenuSystem : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup();
protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* SingleBtn;
	
	UPROPERTY(meta = (BindWidget))
	UButton* MultiBtn;

	UFUNCTION()
	void SingleBtnClicked();
	
	UFUNCTION()
	void MultiBtnClicked();

	FString PathToMap{ TEXT("") };

};
