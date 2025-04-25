#include "BlasterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Blaster/BlasterTypes/CombatState.h"
#include "BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Math/MathCalculation.h"
void UBlasterAnimInstance::NativeInitializeAnimation()
{
	
	Super::NativeInitializeAnimation();

	Character = Cast<ABlasterCharacter>(TryGetPawnOwner());

}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{

	Super::NativeUpdateAnimation(DeltaTime);

	if (Character == nullptr) {
		Character = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (Character == nullptr) return;

	CharacterAminInstance();
	MovementAnimInstance(DeltaTime);
	WeaponAnimInstance();

	FABRIK(DeltaTime);

	//bHoldingForegrip = false; //not available at this point, however still stand there later
	

}

void UBlasterAnimInstance::CharacterAminInstance()
{
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	EquippedWeapon = Character->GetEquippedWeapon();
	CharacterTrajector = Character->GetCharacterTrajector();
	TurningInPlace = Character->GetTurningInPlace();
	FallingTime = Character->GetFallingTime();

	bIsInAir = Character->GetCharacterMovement()->IsFalling();
	bWeaponEquipped = Character->IsWeaponEquipped();
	bIsCrouched = Character->bIsCrouched;
	bAiming = Character->IsAiming();
	bRotateRootBone = Character->ShouldRotateRootBone();
	bElimmed = Character->IsElimmed();
	bDeath = Character->IsDeath();
	bAICharacter = Character->IsAICharacter();
	bMeleeFinished = Character->IsMeleeFinished();
	bRunState = Character->IsRunState();
	bSlideState = Character->IsSlideState();
	bInFirstPerson = Character->IsInFirstPerson();
	bAimingState = Character->IsAimingState();
	bFPPAiming = Character->IsFPPAiming();

	AngleAimingYaw = 0.f;
	AngleAimingPitch = 0.f;
}

void UBlasterAnimInstance::WeaponAnimInstance()
{
	bPistolEquipped = 
		bMeleeWeapon = bHoldingForegrip = 
		bHoldingStock = bExceptionFullBody = 
		bHeavyShoulderWeapon = bHeavyCarriedWeapon =
		bOneHandWeapon  = bLightWeapon = false;

	if (EquippedWeapon)
	{
		bPistolEquipped = EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol;
		bMeleeWeapon = EquippedWeapon->IsMeleeWeapon();
		bHoldingForegrip = EquippedWeapon->IsHoldingForegrip();
		bHoldingStock = EquippedWeapon->IsHoldingStock();
		bHeavyShoulderWeapon = EquippedWeapon->IsHeavyShoulderWeapon();
		bHeavyCarriedWeapon = EquippedWeapon->IsHeavyCarriedWeapon();
		bOneHandWeapon = EquippedWeapon->IsOneHandWeapon();
		bLightWeapon = EquippedWeapon->IsLightWeapon();


		bExceptionFullBody = !(Character->GetCombatState() != ECombatState::ECS_Attack);

		AngleAimingPitch = EquippedWeapon->GetAngleAimingPitch();
		AngleAimingYaw = EquippedWeapon->GetAngleAimingYaw();

		MuzzleToTarget();
	}
}

void UBlasterAnimInstance::MovementAnimInstance(float DeltaTime)
{
	if (Character->IsAICharacter())
	{
		bIsInAccelerating = Character->IsMovementAccelerating();
		//UE_LOG(LogTemp, Warning, TEXT("AI Movement Detect"));
	}
	else
	{
		if (Character->GetCharacterMovement())
		{
			bIsInAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
		}
		
	}
	//UE_LOG(LogTemp, Warning, TEXT("Acceleration = %s"), BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? TEXT("True") : TEXT("False"));

	//Offset Yaw for Strafing
	FRotator AimRotation = Character->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 15.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = Character->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = Character->GetAO_Yaw();
	AO_Pitch = Character->GetAO_Pitch();
}

void UBlasterAnimInstance::FABRIK(float DeltaTime)
{
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && Character->GetMesh())
	{
		bExceptionLeftHandTransform = Character->GetCombatState() == ECombatState::ECS_HitReact ||
			Character->IsRunState() ||
			(Character->GetCharacterMovement() && Character->GetCharacterMovement()->IsFalling());
		if (bExceptionLeftHandTransform)
		{
			FABRIK_LeftHandTransform();
		}
		else
		{
			FABRIK_LeftHandTransform();
			FABRIK_RightHandTransform(DeltaTime);
		}
		
	}

	Apply_FABRIK();
}

void UBlasterAnimInstance::Apply_FABRIK()
{
	bool CharacterReloadState = Character->GetCombatState() != ECombatState::ECS_Reloading;
	bool CharacterAmmoSwitchState = Character->GetCombatState() != ECombatState::ECS_AmmoSwitch;
	bool CharacterHitReactState = Character->GetCombatState() != ECombatState::ECS_HitReact;
	bool CharacterAttackState = Character->GetCombatState() != ECombatState::ECS_Attack;
	bool CharacterSlideState = Character->GetCombatState() != ECombatState::ECS_Slide;
	bool CharacterEquippedRunState = true; //defalut if character no weapon equipped
	if (EquippedWeapon)
	{
		CharacterEquippedRunState = bRunState == false ||
			(bRunState == true &&
			bPistolEquipped == false &&
			bMeleeWeapon == false && 
			bLightWeapon == false);
	}
	bUseFABRIK = bUseAimOffset = bTransformRightHand =
		CharacterReloadState &&
		CharacterAmmoSwitchState &&
		CharacterHitReactState &&
		CharacterSlideState &&
		CharacterAttackState &&
		CharacterEquippedRunState;
}

void UBlasterAnimInstance::FABRIK_RightHandTransform(float DeltaTime)
{
	if (Character->IsLocallyControlled() == false ||
		Character->IsAICharacter()) return;
	
		bLocallyControlled = true;
		RightHandTransform = Character->GetMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
		FRotator LookAtRightRotation = UKismetMathLibrary::FindLookAtRotation(
			RightHandTransform.GetLocation(),
			RightHandTransform.GetLocation() +
			(RightHandTransform.GetLocation() - Character->GetHitTarget())
		);
		float RightRotationYaw = AngleAimingYaw;
		float RightRotationPitch = AngleAimingPitch;

		LookAtRightRotation.Roll += Character->RightHandRotationRoll;
		LookAtRightRotation.Yaw += Character->RightHandRotationYaw + RightRotationYaw;
		LookAtRightRotation.Pitch += Character->RightHandRotationPitch + RightRotationPitch;
		RightHandRotation = FMath::RInterpTo(
			RightHandRotation,
			LookAtRightRotation,
			DeltaTime,
			10000.f);
}

void UBlasterAnimInstance::FABRIK_LeftHandTransform()
{
	LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);

	FVector OutPosition;
	FRotator OutRotation;

	Character->GetMesh()->TransformToBoneSpace(
		FName("hand_r"),
		LeftHandTransform.GetLocation(), //LeftHandTransform.Rotator(),
		FRotator::ZeroRotator,
		OutPosition,
		OutRotation);

	//OutPosition = FVector(OutPosition.X, OutPosition.Y, OutPosition.Z);
	//OutRotation = FRotator(OutRotation.Pitch, OutRotation.Yaw, OutRotation.Roll);
	//this need to change during GL get wrong rotation that disconnect bone (hand_r)
	LeftHandTransform.SetLocation(OutPosition);
	LeftHandTransform.SetRotation(FQuat(OutRotation));
}

void UBlasterAnimInstance::MuzzleToTarget()
{
	FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Muzzle"), ERelativeTransformSpace::RTS_World);
	FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

	//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
	//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), Character->GetHitTarget(), FColor::Orange);
	//TODO: Code for the muzzle -> target
}
