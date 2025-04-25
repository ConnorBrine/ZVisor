// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AttackPressedAnimNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UAttackPressedAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
private:
	bool bPressed = false;
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
