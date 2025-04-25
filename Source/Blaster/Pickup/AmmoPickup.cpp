#include "AmmoPickup.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/GameMode/BlasterGameMode.h"

void AAmmoPickup::Destroyed()
{
	Super::Destroyed();

	ABlasterGameMode* CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (CharacterGameMode)
	{
		CharacterGameMode->DecreaseAmmoNum();
	}
}

void AAmmoPickup::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult
)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	ABlasterCharacter* Character = Cast<ABlasterCharacter>(OtherActor);
	if (Character)
	{
		if (Character->IsAICharacter()) return;
		UCombatComponent* Combat = Character->GetCombat();
		if (Combat)
		{
			Combat->PickupAmmo(WeaponType, AmmoAmount);
		}
	}
	Destroy();
}
