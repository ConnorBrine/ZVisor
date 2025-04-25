#include "MeleeWeaponAnimNotifyState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h" //remove this
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Weapon/MeleeWeapon.h"

void UMeleeWeaponAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			if (Character->GetCombat())
			{
				if (Character->IsWeaponEquipped() && Character->GetEquippedWeapon()->IsMeleeWeapon())
				{
					AMeleeWeapon* WeaponEquipped = Cast<AMeleeWeapon>(Character->GetEquippedWeapon());
					if (WeaponEquipped && WeaponEquipped->IsMeleeWeapon()) WeaponEquipped->AttackStart();
				}
			}
		}
	}
}

void UMeleeWeaponAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			if (Character->GetCombat())
			{
				if (Character->IsWeaponEquipped() && Character->GetEquippedWeapon()->IsMeleeWeapon())
				{
					AMeleeWeapon* WeaponEquipped = Cast<AMeleeWeapon>(Character->GetEquippedWeapon());
					if (WeaponEquipped && WeaponEquipped->IsMeleeWeapon()) WeaponEquipped->AttackEnd();
				}
			}
		}
	}
}
