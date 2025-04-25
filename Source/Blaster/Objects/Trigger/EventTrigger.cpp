#include "EventTrigger.h"

AEventTrigger::AEventTrigger()
{

}

void AEventTrigger::BeginPlay()
{
	Super::BeginPlay();
}

void AEventTrigger::Event(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && OtherActor != this)
	{

	}
}
