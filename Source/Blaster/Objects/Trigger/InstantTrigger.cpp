#include "InstantTrigger.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Kismet/GameplayStatics.h"

AInstantTrigger::AInstantTrigger()
{

	OnActorBeginOverlap.AddDynamic(this, &AInstantTrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AInstantTrigger::OnOverlapEnd);

}

void AInstantTrigger::BeginPlay()
{
	Super::BeginPlay();

	
}

void AInstantTrigger::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	Super::OnOverlapBegin(OverlappedActor, OtherActor);


	if (!bOverlapped && this &&
		OtherActor &&
		OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		bOverlapped = true;
		ABlasterCharacter* Character = Cast<ABlasterCharacter>(OtherActor);
		if (Character && !Character->IsAICharacter())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString(TriggerName));

			Character->TriggerPathChapter(TriggerName);

		}
	}
}

void AInstantTrigger::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	//if (OtherActor)
	//{
	//	Super::OnOverlapEnd(OverlappedActor, OtherActor);
	//	if (this &&
	//		OtherActor &&
	//		OtherActor->GetName() == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetName())
	//	{
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("end"));
			bOverlapped = false;
/*		}
	}*/	
}

void AInstantTrigger::SetNotificationScreenVisibility(bool bVisibility)
{
	Super::SetNotificationScreenVisibility(bVisibility);
}
