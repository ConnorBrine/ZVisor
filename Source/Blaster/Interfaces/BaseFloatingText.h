#pragma once

#define DEFAULT_FINAL_CAUSER_NUMBER 300.f

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Blaster/Weapon/HitboxType.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "BaseFloatingText.generated.h"

UCLASS()
class BLASTER_API ABaseFloatingText : public AActor
{
	GENERATED_BODY()
	
public:	

	ABaseFloatingText();
	virtual void Tick(float DeltaTime) override;



	void SetDamageText(float DamageNumber);
	void SetTextColor(EHitboxType Hitbox);
	void SetCauserCharacter(AActor* NewCauserCharacter);
	void SetFinalCauserLocation();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Text")
	float DamageAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Text", meta = (ExposeOnSpawn = "true"))
	class UTextRenderComponent* DamageText;

protected:
	virtual void BeginPlay() override;

private:
	//class ABlasterCharacter* CauserCharacter;
	class AActor* CauserCharacter;

	FVector FinalCauserLocation;


public:	



};
