#include "Notification.h"

// Sets default values
ANotification::ANotification()
{
	PrimaryActorTick.bCanEverTick = true;

	NotificationScreen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NotificationScreen"));
	NotificationScreen->SetupAttachment(RootComponent);
	NotificationScreen->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NotificationScreen->SetRelativeLocation(FVector(0.f, 0.f, 220.f));
	NotificationScreen->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));

}

void ANotification::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANotification::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANotification::SetNotificationScreenVisibility(bool bVisibility)
{
	NotificationScreen->ToggleVisibility(true);
	NotificationScreen->SetVisibility(true);
	UE_LOG(LogTemp, Error, TEXT("Trigger Message Success"));
}

