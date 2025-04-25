#include "UnarmedAnimNotifyState.h"
#include "Blaster/Character/BlasterCharacter.h"

void UUnarmedAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			Character->MeleeStart();
		}
	}
}

void UUnarmedAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABlasterCharacter* const Character = Cast<ABlasterCharacter>(MeshComp->GetOwner()))
		{
			Character->MeleeEnd();
		}
	}
}
