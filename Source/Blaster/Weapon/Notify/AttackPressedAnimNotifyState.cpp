#include "AttackPressedAnimNotifyState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/BlasterComponents/CombatComponent.h" //remove this
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Weapon/MeleeWeapon.h"

void UAttackPressedAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			if (Character->GetCombat())
			{
				if(Character->GetCombat()->IsFireButtonPressed()) bPressed = true;
			}
		}
	}
}

void UAttackPressedAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			if (Character->GetCombat())
			{
				if (bPressed == false) Character->StopAttack();
				bPressed = false;
			}
		}
	}
}
