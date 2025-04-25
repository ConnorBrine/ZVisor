#include "MeleeAnimNotifyState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h" //remove this
#include "Blaster/Weapon/Weapon.h"

void UMeleeAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			if (Character->GetCombat())
			{
				if (Character->IsWeaponEquipped() && Character->IsRestrictWeapon() == false)
				{
					AWeapon* WeaponEquipped = Character->GetEquippedWeapon();
					if (WeaponEquipped) WeaponEquipped->MeleeStart();
				}
				else
				{
					Character->MeleeStart();
				}
			}		
			else
			{
				Character->MeleeStart();
			}
		}
	}
}

void UMeleeAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			if (Character->GetCombat())
			{
				if (Character->IsWeaponEquipped() && Character->IsRestrictWeapon() == false)
				{
					AWeapon* WeaponEquipped = Character->GetEquippedWeapon();
					if (WeaponEquipped) WeaponEquipped->MeleeEnd();
				}
				else
				{
					Character->MeleeEnd();
				}
			}
			else
			{
				Character->MeleeEnd();
			}
		}
	}
}
