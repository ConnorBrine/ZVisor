#include "Trigger.h"
#include "Components/WidgetComponent.h"

ATrigger::ATrigger()
{
	NotificationScreen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NotificationScreen"));
	NotificationScreen->SetupAttachment(RootComponent);
	NotificationScreen->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NotificationScreen->SetRelativeLocation(FVector(0.f, 0.f, 220.f));
	NotificationScreen->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
}

void ATrigger::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &ATrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ATrigger::OnOverlapEnd);
	NotificationScreen->SetVisibility(true);
	//ObjectiveLocationWidget->SetVisibility(false);
}

void ATrigger::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("true"));
}

void ATrigger::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("false"));
}

void ATrigger::SetNotificationScreenVisibility(bool bVisibility)
{
	NotificationScreen->ToggleVisibility(true);
	NotificationScreen->SetVisibility(true);
	UE_LOG(LogTemp, Error, TEXT("Trigger Message Success"));
}

void ATrigger::SetObjectiveLocationVisibility(bool bVisibility)
{
}

void ATrigger::SetNotificationScreenVisibilityTest(bool Test)
{
	NotificationScreen->SetVisibility(true);
}

FString ATrigger::GetTriggerName()
{
	return TriggerName;
}
