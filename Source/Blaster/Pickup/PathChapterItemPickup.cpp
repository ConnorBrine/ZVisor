#include "PathChapterItemPickup.h"
#include "Blaster/Character/BlasterCharacter.h"

void APathChapterItemPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* Character = Cast<ABlasterCharacter>(OtherActor);
	if (Character && Character->IsAICharacter() == false)
	{
		Character->PickUpObjectiveItem();
		
		if (Character->IsInCollectObjective())
		{
			Destroy();
		}
	}
}


void APathChapterItemPickup::SetNotificationScreenVisibility(bool bVisibility)
{
	Super::SetNotificationScreenVisibility(bVisibility);
}

