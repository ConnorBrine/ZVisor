#include "Pickup.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	SetRootComponent(PickupMesh);

	PickupMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	PickupMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));

	PickupMesh->SetRenderCustomDepth(true);
	PickupMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
	EnableCustomDepth(true);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetSphereRadius(PICKUP_SPHERE_RADIUS);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	OverlapSphere->SetGenerateOverlapEvents(true);
	OverlapSphere->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	NotificationScreen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NotificationScreen"));
	NotificationScreen->SetupAttachment(RootComponent);
	NotificationScreen->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NotificationScreen->SetRelativeLocation(FVector(0.f, 0.f, 220.f));
	NotificationScreen->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
	NotificationScreen->SetVisibility(true);
}
void APickup::BeginPlay()
{
	Super::BeginPlay();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnSphereOverlap);

	//PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//PickupMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* Character = Cast<ABlasterCharacter>(OtherActor);
	if (Character && Character->IsAICharacter()) return;
}

void APickup::EnableCustomDepth(bool bEnable)
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(bEnable);
	}
}

void APickup::SetNotificationScreenVisibility(bool bVisibility)
{
	//NotificationScreen->SetVisibility(bVisibility);
}


void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> Objects;

		GetOverlappingActors(Objects);
		for (AActor* Object : Objects)
		{
			UE_LOG(LogTemp, Warning, TEXT("ALL INFO: %s"), *Object->GetFName().ToString());
		}
	
}

void APickup::Destroyed()
{
	Super::Destroy();

	ABlasterGameMode* CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (CharacterGameMode)
	{
		CharacterGameMode->DecreaseWeaponItemNum();
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}
}

