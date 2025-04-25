#include "BaseFloatingText.h"
#include "Blaster/Weapon/HitboxType.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Character/BlasterCharacter.h"

ABaseFloatingText::ABaseFloatingText()
{
	PrimaryActorTick.bCanEverTick = true;

	DamageAmount = 0.f;

	DamageText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DamageText"));
	DamageText->SetupAttachment(GetRootComponent());
	DamageText->SetText(FText::FromString("0.0"));
	DamageText->SetTextRenderColor(FColor::Red);

}

void ABaseFloatingText::SetDamageText(float DamageNumber)
{
	DamageAmount = DamageNumber;
	DamageText->SetText(FText::FromString(FString::SanitizeFloat(DamageAmount)));

	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, FString::SanitizeFloat(DamageAmount));
	SetLifeSpan(2.f);
}

void ABaseFloatingText::SetTextColor(EHitboxType Hitbox)
{
	switch (Hitbox)
	{
	case EHitboxType::EHT_Head:
		DamageText->SetTextRenderColor(FColor::Yellow);
		break;
	case EHitboxType::EHT_Neck:
		DamageText->SetTextRenderColor(FColor::Orange);
		break;
	case EHitboxType::EHT_Body:
		DamageText->SetTextRenderColor(FColor::Red);
		break;
	default:
		DamageText->SetTextRenderColor(FColor::FromHex(FString("#964B00")));
		break;
	}
}

void ABaseFloatingText::SetCauserCharacter(AActor* NewCauserCharacter)
{
	CauserCharacter = NewCauserCharacter;
}

void ABaseFloatingText::SetFinalCauserLocation()
{
	FinalCauserLocation = FVector(
		GetActorLocation().X + 100.f,
		//FMath::RandRange(-DEFAULT_FINAL_CAUSER_NUMBER, DEFAULT_FINAL_CAUSER_NUMBER),
		GetActorLocation().Y + 100.f,
		//FMath::RandRange(-DEFAULT_FINAL_CAUSER_NUMBER, DEFAULT_FINAL_CAUSER_NUMBER),
		GetActorLocation().Z + 100.f);
		//FMath::RandRange(-DEFAULT_FINAL_CAUSER_NUMBER, DEFAULT_FINAL_CAUSER_NUMBER));
}

void ABaseFloatingText::BeginPlay()
{
	Super::BeginPlay();

	SetFinalCauserLocation();
}

void ABaseFloatingText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this && DamageText && CauserCharacter)
	{
		FVector NewCauserDamageLocation = FMath::VInterpTo(
			GetActorLocation(), FinalCauserLocation, DeltaTime, 10.f);
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, FString("Rotate"));
		//DamageText->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CauserCharacter->GetActorLocation()));
		
	}



}



