#include "FireReloadingAnimNotifyState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h" //remove this
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Weapon/MeleeWeapon.h"

void UFireReloadingAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			if (Character->GetCombat())
			{
				Character->GetCombat()->SetFireReloading(true);
			}
		}
	}
}

void UFireReloadingAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			if (Character->GetCombat())
			{
				Character->GetCombat()->SetFireReloading(false);
			}
		}
	}
}
