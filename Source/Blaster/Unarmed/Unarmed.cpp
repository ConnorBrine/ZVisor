#include "Unarmed.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"

AUnarmed::AUnarmed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AUnarmed::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUnarmed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUnarmed::Attack()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	AController* InstigatorController = OwnerPawn->GetController();
	FHitResult AttackHit;
		ABlasterCharacter* VictimCharacter = Cast<ABlasterCharacter>(AttackHit.GetActor());
		if (VictimCharacter && HasAuthority() && InstigatorController)
		{
			UE_LOG(LogTemp, Warning, TEXT("Victim damage begin"));
			UGameplayStatics::ApplyDamage(
				VictimCharacter,
				Damage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}

		//if (HitSound)
		//{
		//	UGameplayStatics::PlaySoundAtLocation(
		//		this,
		//		HitSound,
		//		FireHit.ImpactPoint
		//	);
		//}

	
}

