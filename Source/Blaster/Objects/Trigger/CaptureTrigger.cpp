#include "CaptureTrigger.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Kismet/GameplayStatics.h"

ACaptureTrigger::ACaptureTrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &ACaptureTrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ACaptureTrigger::OnOverlapEnd);
}
void ACaptureTrigger::BeginPlay()
{
	Super::BeginPlay();



}

void ACaptureTrigger::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	Super::OnOverlapBegin(OverlappedActor, OtherActor);

	if (!bOverlapped &&
		OtherActor && 
		OtherActor != this && 
		OtherActor->GetName() == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetName())
	{
		bOverlapped = true;

		ABlasterCharacter* Character = Cast<ABlasterCharacter>(OtherActor);
		if (Character && !Character->IsAICharacter())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString("Inside"));
			Character->TriggerPathChapter(TriggerName);
		}
	}
}

void ACaptureTrigger::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	Super::OnOverlapEnd(OverlappedActor, OtherActor);

	if (bOverlapped && 
		OtherActor &&
		OtherActor != this &&
		OtherActor->GetName() == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetName())
	{
		bOverlapped = false;

		ABlasterCharacter* Character = Cast<ABlasterCharacter>(OtherActor);
		if (Character && !Character->IsAICharacter())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString("Outside"));
			Character->TriggerPathChapter(FString("null"));
		}
	}

	
}

void ACaptureTrigger::SetNotificationScreenVisibility(bool bVisibility)
{
	Super::SetNotificationScreenVisibility(bVisibility);
}
