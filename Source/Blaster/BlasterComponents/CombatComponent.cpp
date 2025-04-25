#include "CombatComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/BlasterTypes/Constant.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Spawn/SpawnController.h"
#include "Blaster/Weapon/Weapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


	//this below is default value which can be change base your gameplay
	RunSpeed = 700.f;
	SlideSpeed = 500.f;
	BaseWalkSpeed = 300.f;
	AimWalkSpeed = 75.f;
	BaseReduce = 0.f;

	LightWeaponEquippedSpeedReduce = 0.f;
	WeaponEquippedSpeedReduce = 50.f;
	HeavyWeaponEquippedSpeedReduce = 225.f;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, EquippedSecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
}

void UCombatComponent::SetHitTargetWithCrosshair(float DeltaTime)
{
	FHitResult HitResult;

	if (Character->IsAICharacter())
	{
		AAICharacterController* AIController = Cast<AAICharacterController>(Character->GetController());
		if (AIController && AIController->GetTargetCharacter())
		{
			AIAimingRange(AIController->GetTargetCharacterLocation());
		}
	}
	//apply with AI Character only (with return Character Target Close to ....

	else
	{
		if (IsExceptionHitResult())
		{
			TraceUnderADSWeapon(HitResult);
			HitTarget = HitResult.ImpactPoint;
			ClearHUDCrosshairs(DeltaTime);
		}
		//exception for running, jumping

		else if (Character->IsFPPAiming())
		{
			//TraceUnderADSWeapon(HitResult);
			TraceUnderCrosshairs(HitResult);
			//wrong angle
			HitTarget = HitResult.ImpactPoint;
			ClearHUDCrosshairs(DeltaTime);
		}
		//apply with ADS Weapon in First Person Perspective (Scope, Iron Sight) 

		else
		{
			TraceUnderCrosshairs(HitResult);
			HitTarget = HitResult.ImpactPoint;
			SetHUDCrosshairs(DeltaTime);

		}
	}

	if (EquippedWeapon)
	{
		//DrawDebugSphere(GetWorld(), HitTarget, 12.f, 12, FColor::Red);
		//DrawDebugLine(
		//	GetWorld(),
		//	EquippedWeapon->GetWeaponMuzzleTransform(ERelativeTransformSpace::RTS_World).GetLocation(),
		//	HitTarget,
		//	FColor::Blue);
	}

	//apply with hip-fire and Aiming (Third Person Perspective only)



	//UE_LOG(LogTemp, Warning, TEXT("HitTarget: %f %f %f"), HitTarget.X, HitTarget.Y, HitTarget.Z);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;


		if (Character->GetTPPCamera())
		{
			DefaultFOV = Character->GetTPPCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		if (Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}

		LastTargetLocation = Character->GetTargetLocation();
	}

	NoEquippedWeapon = 0;
	NoInputChange = 0;

	SetupHandAttachObject();

}

void UCombatComponent::Running()
{

	if (Character)
	{
		if (Character->bIsCrouched)
		{
			Character->UnCrouch();
		}

		if (Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f)
		{
		

			Character->GetCharacterMovement()->MaxWalkSpeed = 
				FMath::Clamp(RunSpeed - GetCharacterMovementReduce(), 0, RunSpeed);
		}
		if (Character->IsAiming())
		{
			SetAiming(false);
		}

		PlayRunningSound();
	}
}

void UCombatComponent::StopRunning()
{
	if (Character)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Stop run now"));
		Character->GetCharacterMovement()->MaxWalkSpeed = 
			FMath::Clamp(BaseWalkSpeed - GetCharacterMovementReduce(), 0, BaseWalkSpeed);
			BaseWalkSpeed;
		if (Character->IsAiming())
		{
			SetAiming(true);
		}
		Character->SetRunningTimer();
	}
}



void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character)
	{
		SetHitTargetWithCrosshair(DeltaTime);

		InterpFOV(DeltaTime);

		SetCharacterSpeed();
	}
	if (RecoilTimeLine.IsPlaying())
	{
		RecoilTimeLine.TickTimeline(DeltaTime);
	}
	if (RecoilTimeLine.IsReversing())
	{
		ResetRecoil(DeltaTime);
	}
}

void UCombatComponent::SetCharacterSpeed()
{
	float Speed =
		(bRunButtonPressed && (bAiming == false)) ?
		FMath::Clamp(RunSpeed - GetCharacterMovementReduce(), 0, RunSpeed) :
		Character->IsSlideState() ?
		FMath::Clamp(SlideSpeed - GetCharacterMovementReduce(), 0, SlideSpeed) :
		FMath::Clamp(BaseWalkSpeed - GetCharacterMovementReduce(), 0, BaseWalkSpeed);

	Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] += AmmoAmount;
		//was FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoAmount, 0, MaxCarriedAmmo)

		UpdateCarriedAmmo();
	}
	if (EquippedWeapon && EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		Reload();
	}
}

void UCombatComponent::ApplyWeaponEquippedRecoil()
{
	FOnTimelineFloat XRecoilCurve;
	FOnTimelineFloat YRecoilCurve;

	XRecoilCurve.Clear();
	YRecoilCurve.Clear();

	XRecoilCurve.BindUFunction(this, FName("StartHorizonalRecoil"));
	YRecoilCurve.BindUFunction(this, FName("StartVerticalRecoil"));

	if (EquippedWeapon && EquippedWeapon->GetHorizonalCurve() && EquippedWeapon->GetVerticalCurve() )
	{
		
		RecoilTimeLine.AddInterpFloat(EquippedWeapon->GetHorizonalCurve(), XRecoilCurve);
		RecoilTimeLine.AddInterpFloat(EquippedWeapon->GetVerticalCurve(), YRecoilCurve);
	}
}

void UCombatComponent::AIAimingRange(FVector TargetHit)
{
	TargetHit = FVector(
		FMath::RandRange(TargetHit.X - AI_SHOOTING_RANGE_VERY_EASY, TargetHit.X + AI_SHOOTING_RANGE_VERY_EASY),
		FMath::RandRange(TargetHit.Y - AI_SHOOTING_RANGE_VERY_EASY, TargetHit.Y + AI_SHOOTING_RANGE_VERY_EASY),
		FMath::RandRange(TargetHit.Z - AI_SHOOTING_RANGE_VERY_EASY, TargetHit.Z + AI_SHOOTING_RANGE_VERY_EASY));

	HitTarget = TargetHit;
}



void UCombatComponent::FireButtonPressed(bool bPressed)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	
	bFireButtonPressed = bPressed;
	
	if (EquippedWeapon->IsMeleeWeapon()) MeleeWeaponButtonPressed();
	else RangeWeaponButtonPressed();


}

void UCombatComponent::RangeWeaponButtonPressed()
{
	//if (Character && Character->GetCombatState() == ECombatState::ECS_Reloading && )
	//{
	//	return;
	//}

	if (bFireButtonPressed)
	{
		Fire();

	}
	else
	{
		bFirstFire = false;
		ReverseRecoil();
	}
	StartFireTimer();
}

void UCombatComponent::MeleeWeaponButtonPressed()
{
	if (Character && bFireButtonPressed)
	{
		Attack();
	}
}

void UCombatComponent::RunButtonPressed(bool bPressed)
{
	if (Character == nullptr) return;


	bRunButtonPressed = bPressed;
	if (bAiming && 
		Character->IsMovingForward() && Character->IsRunPressed()
		)
	{
		SetAiming(false);
	}
	if (bRunButtonPressed && 
		Character->IsMovingForward() && Character->IsRunPressed()
		)
	{
		Running();
		Character->SetCharacterRunState(true);
	}
	else if (bRunButtonPressed == false ||
		(Character->IsMovingForward() && !Character->IsRunPressed()) ||
		(!Character->IsMovingForward() && Character->IsRunPressed())
		)
	{
		StopRunning();
		Character->SetCharacterRunState(false);
	}

}

void UCombatComponent::Fire()
{
	if (Character->IsAICharacter() == false&&
		EquippedWeapon && 
		EquippedWeapon->IsEmpty())
	{
		//this only allow player getting empty sound, not AI 
		//for annoy reason
		PlayEmptyWeaponSound();
		//return;
	}
	if (CanFire())
	{
		//if(Character->IsAICharacter() == false)
		//	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("Fire Enable"));

		StartRecoil();
		if (!bFirstFire && Character)
		{
			RecoilStartRotation = Character->GetControlRotation();
			bFirstFire = true;
		}
		bCanFire = false;
		ServerFire(HitTarget);
		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 1.2f;
		}
		StartFireTimer();
	}

}

void UCombatComponent::Attack()
{
	ServerAttack();
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr ||
		!bTimeFinished || bIsDown) return;

	bTimeFinished = false;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->FireDelay);

}

//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue,
//	FString(
//		FString::SanitizeFloat(RecoilStartRotation.Roll)
//	) +
//	FString(
//		FString::SanitizeFloat(RecoilStartRotation.Pitch)
//	) +
//	FString(
//		FString::SanitizeFloat(RecoilStartRotation.Yaw)
//	)

//);

void UCombatComponent::FireTimerFinished()
{
	bCanFire = true;
	bTimeFinished = true;

	if (EquippedWeapon == nullptr) return;
	if (bIsDown) return;

	if (bFireButtonPressed && EquippedWeapon && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	if (EquippedWeapon && EquippedWeapon->IsEmpty())
	{
		ReverseRecoil();
		Reload();
		
	}
	if (EquippedWeapon && EquippedWeapon->IsMultiBarrel() && EquippedWeapon->IsAllowedAmmoSwitch() && EquippedWeapon->IsBarrelEmpty())
	{		
		ReverseRecoil();
		AmmoSwitch();
	}

}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::ServerAttack_Implementation()
{
	MulticastAttack();
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character)
	{
		bool AmmoSwitchState = CombatState == ECombatState::ECS_Reloading &&
			EquippedWeapon->IsMultiBarrel() &&
			EquippedWeapon->IsBarrelEmpty();

		if (AmmoSwitchState)
		{
			HandleAmmoSwitch();
			EquippedWeapon->AmmoSwitchWeapon();
			CombatState = ECombatState::ECS_AmmoSwitch;		
			return;
		}

		bool ReloadInterrupt = 
			CombatState == ECombatState::ECS_Reloading
			&& EquippedWeapon->IsUsingMagazine() == false;
		bool WeaponCanFire = CombatState == ECombatState::ECS_Unoccupied;

		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Black, FString(ReloadInterrupt ? "true" : "false"));

		if (ReloadInterrupt || WeaponCanFire)
		{
			Character->PlayFireMontage(bAiming);
			EquippedWeapon->Fire(TraceHitTarget);
			CombatState = ECombatState::ECS_Unoccupied;
		}
	}
}

void UCombatComponent::MulticastAttack_Implementation()
{
	if (EquippedWeapon == nullptr) return;
	if (Character)
	{
		if (CombatState == ECombatState::ECS_Unoccupied)
		{
			CombatState = ECombatState::ECS_Attack;
			Character->PlayAttackMontage(bAiming);
		}
	}
}

void UCombatComponent::PickupWeapon(AWeapon* WeaponToPickup)
{
	if (WeaponToPickup == nullptr) return;
	NoEquippedWeapon++; //some bug that weapon will stack, so this will resolve and helpful later


	if (WeaponToPickup->GetWeaponType() == EWeaponType::EWT_Pistol)
	{
		PickupSecondaryWeapon(WeaponToPickup);
	}
	else
	{
		PickupPrimaryWeapon(WeaponToPickup);
	}

	PlayPickupWeaponSound();
}

void UCombatComponent::PickupPrimaryWeapon(AWeapon* WeaponToPickup)
{
	DropEquippedPrimaryWeapon();
	EquippedPrimaryWeapon = WeaponToPickup;
	EquippedPrimaryWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	EquippedPrimaryWeapon->SetOwner(Character);

	if (EquippedSecondaryWeapon == nullptr)
	{
		EquipPrimaryWeapon();
	}
	else
	{
		AttachPrimaryWeaponToBody(WeaponToPickup);
	}
}

void UCombatComponent::PickupSecondaryWeapon(AWeapon* WeaponToPickup)
{
	DropEquippedSecondaryWeapon();
	
	EquippedSecondaryWeapon = WeaponToPickup;
	EquippedSecondaryWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	EquippedSecondaryWeapon->SetOwner(Character);
	if (EquippedPrimaryWeapon == nullptr)
	{
		
		EquipSecondaryWeapon();
	}
	else
	{
		AttachSecondaryWeaponToBody(WeaponToPickup);
	}
	

}

void UCombatComponent::EquipWeapon()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	//if (CombatState != ECombatState::ECS_Unoccupied) return; // check later

	EquippedWeapon->SetHUDAmmo();

	UpdateCarriedAmmo();

	PlayEquipWeaponSound();

	ReloadEmptyWeapon(EquippedWeapon);

	NoInputChange++;


	//if (EquippedWeapon == nullptr )
	//{
	//	EquipPrimaryWeapon(WeaponToEquip);
	//}
	//else 
	//{
	//	if (EquippedSecondaryWeapon == nullptr)
	//	{
	//		EquipSecondaryWeapon(WeaponToEquip);
	//	}
	//	else
	//	{
	//		
	//	}
	//}
	//function will use if player choose 2 primary weapon
	

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
	
	if (EquippedWeapon->IsMeleeWeapon()) return; //gate for melee waepon
	ApplyWeaponEquippedRecoil(); //check, this will issue will secondary weapon
}

void UCombatComponent::EquipPrimaryWeapon()
{
	if (EquippedWeapon != nullptr)
	{
		AttachSecondaryWeaponToBody(EquippedWeapon);
		EquippedWeapon = nullptr;
	}
	
	AttachWeaponToRightHand(EquippedPrimaryWeapon);
	EquipWeapon();
	WeaponEquippedNum = 1;
}

void UCombatComponent::EquipSecondaryWeapon()
{
	if (EquippedWeapon != nullptr)
	{
		AttachPrimaryWeaponToBody(EquippedWeapon);
		EquippedWeapon = nullptr;
	}
	
	AttachWeaponToRightHand(EquippedSecondaryWeapon);
	EquipWeapon();
	WeaponEquippedNum = 2;
}

void UCombatComponent::UnequippedWeapon()
{
	if (EquippedWeapon == nullptr) return;

	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
	{
		AttachSecondaryWeaponToBody(EquippedWeapon);
	}
	else
	{
		AttachPrimaryWeaponToBody(EquippedWeapon);
	}
	EquippedWeapon = nullptr;
	WeaponEquippedNum = 0;

	StopReloadAndAmmoSwitch();

}

void UCombatComponent::SetupHandAttachObject()
{
	ABlasterGameMode* CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (CharacterGameMode)
	{
		AmmoAttach = CharacterGameMode->AmmoAttachData;
		MagAttach = CharacterGameMode->MagAttachData;
	}
}

void UCombatComponent::AttachWeaponToRightHand(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	EquippedWeapon = WeaponToEquip;

	FName SocketName = FName("RightHandSocket");
	if (EquippedWeapon->IsHoldingStock()) SocketName = FName("StockRightHandSocket");
	if (EquippedWeapon->IsMeleeWeapon()) 
		SocketName = FName("MeleeRightHandSocket"); 

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
}

void UCombatComponent::AttachPrimaryWeaponToBody(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	FName BodyWeaponName = WeaponToEquip->IsMeleeWeapon() ? FName("MeleeWeaponSocket") : FName("PrimaryWeaponSocket");
	const USkeletalMeshSocket* BodySocket = Character->GetMesh()->GetSocketByName(BodyWeaponName);
	if (BodySocket)
	{
		BodySocket->AttachActor(WeaponToEquip, Character->GetMesh());
	}
}

void UCombatComponent::AttachSecondaryWeaponToBody(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	const USkeletalMeshSocket* BodySocket = Character->GetMesh()->GetSocketByName(FName("SecondaryWeaponSocket"));
	if (BodySocket)
	{
		BodySocket->AttachActor(WeaponToEquip, Character->GetMesh()); 

	}
}

void UCombatComponent::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		switch (WeaponEquippedNum)
		{
		case 1:
			DropEquippedPrimaryWeapon();
			break;
		case 2:
			DropEquippedSecondaryWeapon();
			break;
		}
	}
}

void UCombatComponent::DropEquippedPrimaryWeapon()
{
	if (EquippedPrimaryWeapon)
	{
		bWeaponDrop = true;
		EquippedPrimaryWeapon->Dropped();
		EquippedPrimaryWeapon = nullptr;
		EquippedWeapon = nullptr;

	}
}

void UCombatComponent::DropEquippedSecondaryWeapon()
{
	if (EquippedSecondaryWeapon)
	{
		bWeaponDrop = true;
		EquippedSecondaryWeapon->Dropped();
		EquippedSecondaryWeapon = nullptr;
		EquippedWeapon = nullptr;

	}
}

void UCombatComponent::PlayRunningSound()
{
	UGameplayStatics::PlaySoundAtLocation(
		this,
		Character->RunSound,
		Character->GetActorLocation()
	);
}

void UCombatComponent::PlayPickupWeaponSound()
{
	if (EquippedWeapon == nullptr) return;
	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->PickupSound,
			Character->GetActorLocation()
		);
	}
}

void UCombatComponent::PlayEquipWeaponSound()
{
	if (EquippedWeapon == nullptr) return;
	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->EquipSound,
			Character->GetActorLocation()
		);
	}
}

void UCombatComponent::PlayEmptyWeaponSound()
{
	if (EquippedWeapon == nullptr) return;
	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->EmptySound,
			Character->GetActorLocation()
		);
	}
}

void UCombatComponent::ReloadEmptyWeapon(AWeapon* WeaponToReload)
{
	if (WeaponToReload == nullptr) return;

	if (WeaponToReload->IsEmpty())
	{
		Reload();
	}

	if (WeaponToReload->IsMultiBarrel() && WeaponToReload->IsAllowedAmmoSwitch() && WeaponToReload->IsBarrelEmpty())
	{
		AmmoSwitch();
	}
}

void UCombatComponent::AmmoSwitch()
{
	if (CombatState != ECombatState::ECS_AmmoSwitch)
	{		
		ServerAmmoSwitch();
	}
	
}

void UCombatComponent::SingleAmmoReload()
{
	if (Character && Character->HasAuthority())
	{
		UpdateSingleAmmoValue();
	}
}

void UCombatComponent::Reload()
{
	if (EquippedWeapon)
	{
		//bool IsMagCapacity = EquippedWeapon->GetAmmo() == EquippedWeapon->GetMagCapacity();
		bool IsMagCapacity = EquippedWeapon->IsMagCapacity();

		if (!IsMagCapacity && CarriedAmmo > 0 && CombatState != ECombatState::ECS_Reloading)
		{
			bFirstFire = false;
			ServerReload();
			
			if (bFireButtonPressed)
			{
				ReverseRecoil();
			}

		}
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	CombatState = ECombatState::ECS_Reloading;
	HandleReload();
	EquippedWeapon->ReloadWeapon();
}

void UCombatComponent::ServerAmmoSwitch_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	CombatState = ECombatState::ECS_AmmoSwitch;
	HandleAmmoSwitch();
	EquippedWeapon->AmmoSwitchWeapon();
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	if (Character->HasAuthority())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Updated Ammo"));
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValue();
		if (EquippedWeapon)
		{
			if (EquippedWeapon->IsMultiBarrel() && EquippedWeapon->IsAllowedAmmoSwitch() && EquippedWeapon->IsBarrelEmpty())
			{
				AmmoSwitch();
			}
			else if (!EquippedWeapon->IsAllowedAmmoSwitch())
			{
				RestoreBarrelValue();
			}

			EquippedWeapon->ResetAmmoRefillHand();
		}
	}
	if (bFireButtonPressed)
	{
		StartRecoil();
		Fire();

	}
	
}

void UCombatComponent::FinishAmmoSwitch()
{
	//UE_LOG(LogTemp, Warning, TEXT("Should be run there"));
	if (Character == nullptr) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		RestoreBarrelValue();

		if (EquippedWeapon && EquippedWeapon->IsEmpty())
		{
			Reload();
		}
	}
	if (bFireButtonPressed)
	{
		StartRecoil();
		Fire();
	}
}

void UCombatComponent::FinishHitReact()
{

	if (Character == nullptr) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;	
	}
	if (bFireButtonPressed)
	{
		StartRecoil();
		Fire();
	}
}

void UCombatComponent::FinishAttack()
{
	if (Character == nullptr) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
	Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void UCombatComponent::FinishSlide()
{
	if (Character == nullptr) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
	Character->Crouch();
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast < ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::UpdateAmmoValue() 
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast < ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-ReloadAmount);
	//UE_LOG(LogTemp, Warning, TEXT("Reload= %d"), ReloadAmount);
	
}

void UCombatComponent::UpdateSingleAmmoValue()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	if (EquippedWeapon->IsUsingMagazine()) return;

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast < ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-1);
	if (EquippedWeapon->IsFull() || CarriedAmmo == 0)
	{
		Character->StopSingleReloadMontage();
	}
}

void UCombatComponent::RestoreBarrelValue()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	int32 BarrelRestore = RestoreAmmoSwitch();

	if (EquippedWeapon->IsMultiBarrel())
	{
		EquippedWeapon->AddBarrelAmmo(BarrelRestore);
	}
}

void UCombatComponent::StartHorizonalRecoil(float Value)
{
	//debug
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan,
	//	FString::SanitizeFloat(Value) + " - " + FString::SanitizeFloat(RecoilYaw));

	if (RecoilTimeLine.IsReversing()) return;

	if (Character)
	{
		
		GenerateRecoilVector();
		Character->AddControllerYawInput((float) (Value - RecoilYaw) / NoInputChange);
	}
}

void UCombatComponent::StartVerticalRecoil(float Value)
{
	if (RecoilTimeLine.IsReversing()) return;

	if (Character)
	{
		GenerateRecoilVector();
		Character->AddControllerPitchInput((float)(Value - RecoilPitch) / NoInputChange);
	}
}

void UCombatComponent::StartRecoil()
{

	{
		RecoilTimeLine.PlayFromStart();
	}
}

void UCombatComponent::ReverseRecoil()
{
	RecoilTimeLine.ReverseFromEnd();
}

const EWeaponComponent UCombatComponent::GetWeaponComponentAffect()
{
	if (CombatState == ECombatState::ECS_Reloading) return EWeaponComponent::EWC_Magazine;
	//{
	//	//if (EquippedWeapon && EquippedWeapon->IsUsingMagazine())
	//		//return EWeaponComponent::EWC_EjectionPort; //bolt-action, shotgun
	//	 //by default, all weapon using magazine
	//	//}
	//	//Equip Weapon Component, check which component is affect is below
	//	//however, this function will add more code in the future

	//}
	return EWeaponComponent();
}

float UCombatComponent::GetCharacterMovementState()
{
	switch (CharacterMovementState)
	{
		case ECharacterMovementState::ECMS_Run: 
			return RunSpeed;
		case ECharacterMovementState::ECMS_Slide:
			return SlideSpeed;
		case ECharacterMovementState::ECMS_Walk:
			return RunSpeed;
		case ECharacterMovementState::ECMS_Aim:
			return AimWalkSpeed;
	}
	return 0;
}

float UCombatComponent::GetCharacterMovementReduce()
{
	if (EquippedWeapon)
	{
		if (EquippedWeapon->IsHeavyCarriedWeapon() ||
			EquippedWeapon->IsHeavyShoulderWeapon())
			return HeavyWeaponEquippedSpeedReduce;

		if (EquippedWeapon->IsLightWeapon() ||
			EquippedWeapon->IsOneHandWeapon())
			return LightWeaponEquippedSpeedReduce;

		return WeaponEquippedSpeedReduce;
	}
	return BaseReduce;
}

bool UCombatComponent::IsAvailableCarriedAmmoWeapon()
{
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
		return CarriedAmmoMap[EquippedWeapon->GetWeaponType()] > 0;
	return false;
}

void UCombatComponent::GenerateRecoilVector()
{
	if (EquippedWeapon)
	{
		float Angle = FMath::RandRange(EquippedWeapon->GetRecoilMaxLeft(), -EquippedWeapon->GetRecoilMaxRight()); //rand angle
		float tempMag = -FMath::RandRange(
			EquippedWeapon->GetRecoilMagnitude() * EquippedWeapon->GetRecoilMinMultiplier(), 
			EquippedWeapon->GetRecoilMagnitude()
		); //temp Magnitude
		RecoilYaw = FMath::Sin(FMath::DegreesToRadians(Angle)) * tempMag;
		RecoilPitch = -FMath::Cos(FMath::DegreesToRadians(Angle)) * tempMag;
	}

}

void UCombatComponent::ResetRecoil(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;
	if (EquippedWeapon->IsEmpty() || 
		EquippedWeapon->IsFull()) return;


	if (FMath::Abs(Character->GetYawInput()) > 0.f || FMath::Abs(Character->GetPitchInput()) > 0.f)
	{
		RecoilTimeLine.Stop();
	}
	RecoilTimeLine.TickTimeline(DeltaTime);

	FRotator NewRotation = FMath::RInterpTo(Character->GetControlRotation(), RecoilStartRotation, DeltaTime, 6.f);

	if (NewRotation == RecoilStartRotation) bResetRecoil = false;
	Character->Controller->ClientSetRotation(NewRotation);
}

void UCombatComponent::HandAttachMagazine(const USkeletalMeshSocket* HandSocket, FString WeaponName)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	FTransform HandTransform = HandSocket->GetSocketTransform(Character->GetMesh());
	//editor
	//CurrentHandAttachObject = USpawnController::SpawnActorHandAttachByPathName(
	//	GetWorld(),
	//	true,
	//	"Weapon/Magazine",
	//	"BP_" + WeaponName + "_Mag",
	//	HandTransform.GetLocation(), HandTransform.Rotator());

	for (AActor* Mag : MagAttach)
	{
		if (Mag->GetName().Contains(WeaponName))
		{
			CurrentHandAttachObject = USpawnController::SpawnActorHandAttachByActor(
				GetWorld(),
				Mag,
				HandTransform.GetLocation(), HandTransform.Rotator());
			break;
		}
	}


	if (CurrentHandAttachObject)
	{
		HandSocket->AttachActor(CurrentHandAttachObject, Character->GetMesh());
	}
}

void UCombatComponent::HandAttachAmmo(const USkeletalMeshSocket* HandSocket, FString WeaponName)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	FTransform HandTransform = HandSocket->GetSocketTransform(Character->GetMesh());
	//editor
	//CurrentHandAttachObject = USpawnController::SpawnActorHandAttachByPathName(
	//	GetWorld(),
	//	true,
	//	"Weapon/Ammo",
	//	"BP_" + WeaponName + "_Ammo",
	//	HandTransform.GetLocation(), HandTransform.Rotator());

	for (AActor* Ammmo : AmmoAttach)
	{
		if (Ammmo->GetName().Contains(WeaponName))
		{
			CurrentHandAttachObject = USpawnController::SpawnActorHandAttachByActor(
			GetWorld(),
			Ammmo,
			HandTransform.GetLocation(), HandTransform.Rotator());
			break; 
		}
	}

	if (CurrentHandAttachObject)
	{
		HandSocket->AttachActor(CurrentHandAttachObject, Character->GetMesh());
	}
}

void UCombatComponent::HandAttachObject(bool bFirstMesh)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(
		FName("LeftHandSocket"));
	if (HandSocket)
	{
		if (CurrentHandAttachObject) return;
		FString WeaponName = EquippedWeapon->GetClass()->GetName();
		WeaponName.RemoveFromStart("BP_");
		int32 IndexRemove = WeaponName.Find("_");
		if (IndexRemove != -1)
		{
			WeaponName.RemoveAt(IndexRemove, WeaponName.Len() - 1, EAllowShrinking::No);
		}
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString(WeaponName));

		EquippedWeapon->IsUsingMagazine() ? 
			HandAttachMagazine(HandSocket, WeaponName) : HandAttachAmmo(HandSocket, WeaponName);

		if(CurrentHandAttachObject) CurrentHandAttachObject->ShowFirstMesh(bFirstMesh);
	}

	
}

void UCombatComponent::HandDetachObject()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	AHandAttachObject* TempActor = CurrentHandAttachObject;
	if (TempActor)
	{
		TempActor->DetachObject();
		TempActor->SetSimulation(true);
		TempActor->SetLifeSpan(3.f);
	}
	CurrentHandAttachObject = nullptr;

}

void UCombatComponent::DestroyHandAttach()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	AHandAttachObject* TempActor = CurrentHandAttachObject;
	if (TempActor)
	{
		TempActor->DetachObject();
		TempActor->SetLifeSpan(0.001f);
		
	}
	CurrentHandAttachObject = nullptr;

}

void UCombatComponent::AttachWeaponComponent(EWeaponComponent WeaponComponent)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	EquippedWeapon->AttachWeaponComponent(WeaponComponent);
}

void UCombatComponent::DetachWeaponComponent(EWeaponComponent WeaponComponent)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	EquippedWeapon->DetachWeaponComponent(WeaponComponent);
}

void UCombatComponent::SetFireReloading(bool bActiveFire)
{
	bCanFire = bActiveFire;
}

void UCombatComponent::Melee()
{
	if (Character == nullptr) return;
	Character->PlayMeleeMontage();
}

void UCombatComponent::SetHitReactState()
{
	if (Character == nullptr) return;
	CombatState = ECombatState::ECS_HitReact;
}

void UCombatComponent::StopReloadAndAmmoSwitch()
{
	if (Character == nullptr) return;

	if (CombatState == ECombatState::ECS_Reloading)
	{
		Character->StopReloadMontage();
	}
	if (CombatState == ECombatState::ECS_AmmoSwitch)
	{
		Character->StopAmmoSwitchMontage();
	}
}

void UCombatComponent::SetSlideState()
{
	if (Character == nullptr) return;
	CombatState = ECombatState::ECS_Slide;
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading: 
		HandleReload();
		break;
	case ECombatState::ECS_AmmoSwitch:
		HandleAmmoSwitch();
		break;
	case ECombatState::ECS_Unoccupied:
	//case ECombatState::ECS_HitReact:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_Attack:
		//use this later
		break;
	case ECombatState::ECS_Slide:
		break;
	}
}

bool UCombatComponent::IsExceptionHitResult()
{
	return CombatState == ECombatState::ECS_HitReact ||
		Character->IsRunState() ||
		(Character->GetCharacterMovement() && Character->GetCharacterMovement()->IsFalling());
}

void UCombatComponent::HandleReload()
{
	if (Character == nullptr) return;
	Character->PlayReloadMontage();
}

void UCombatComponent::HandleAmmoSwitch()
{
	if (Character == nullptr) return;
	Character->PlayAmmoSwitchMontage();
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(AmountCarried, RoomInMag);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::CheckAmmoReload()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	if (EquippedWeapon->IsUsingMagazine()) return;
	//if (CCharacter->IsAICharacter()) return;

	EquippedWeapon->ApplyAmmoRefillHand();

	if (EquippedWeapon->IsFull() == false && CarriedAmmo != 0)
	{
		if (EquippedWeapon->IsEmptyAmmoRefillHand())
		{
			Character->PlaySingleReloadMontage();
		}
	}

}

int32 UCombatComponent::RestoreAmmoSwitch()
{
	if (EquippedWeapon == nullptr) return 0;
	int32 CurrentCapacity = EquippedWeapon->GetAmmo();
	//UE_LOG(LogTemp, Warning, TEXT("Restore: %d"), FMath::Clamp(CurrentCapacity, 0, EquippedWeapon->GetBarrel()));
		return FMath::Clamp(CurrentCapacity, 0, EquippedWeapon->GetBarrel());
}



void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		FString FNameSocket = "RightHandSocket";
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(
			EquippedWeapon->IsHoldingStock() ? FName("StockRightHandSocket") : FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
		if (EquippedWeapon->EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				EquippedWeapon->EquipSound,
				Character->GetActorLocation()
			);
		}
	}
}

void UCombatComponent::OnRep_EquippedPrimaryWeapon()
{
}
void UCombatComponent::OnRep_EquippedSecondaryWeapon()
{
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	//if (Character->IsAICharacter())
	//{
	//	TraceAICharacter(TraceHitResult);
	//	return;
	//}
	FVector2D ViewportSize = FVector2D();
	if (GEngine && GEngine->GameViewport) 
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
			
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor::Red
			);
		}
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}

	}


}

void UCombatComponent::TraceUnderADSWeapon(FHitResult& TraceHitResult)
{
	if (EquippedWeapon == nullptr) return;
	FVector SightDirection = EquippedWeapon->GetWeaponMuzzleTransform(ERelativeTransformSpace::RTS_World).GetRotation().Vector();



	FVector Start = EquippedWeapon->GetWeaponAimSightTransform(ERelativeTransformSpace::RTS_World).GetLocation();

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += SightDirection * (DistanceToCharacter + 100.f);

		}

		FVector End = Start + SightDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}
}

void UCombatComponent::TraceAICharacter(FHitResult& TraceHitResult)
{

	//bool IsSightOfPlayer = AAIController::LineOfSightTo(PlayerPawn);
	if (Character->IsSightOfPlayer())
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		FVector TargetLocation = PlayerPawn->GetTargetLocation();
		FVector Start;
		if (Character)
		{
			Start = Character->GetActorLocation();

		}

		//GetWorld()->LineTraceSingleByChannel(
		//	TraceHitResult,
		//	Start,
		//	TargetLocation,
		//	ECollisionChannel::ECC_Visibility
		//);

		

	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;
	if (Character->IsFPPAiming()) return;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;
		if(HUD)
		{

			if (EquippedWeapon)
			{
				
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;				
			}
			else
			{
				
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				
			}
			//Calculate crosshair spread

			//[0, 600] -> [0, 1]
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;
			
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, -0.56f, DeltaTime, 30.f);
			}
			else 
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 7.f);

			HUDPackage.CrosshairSpread = 
				0.5f +
				CrosshairVelocityFactor + 
				CrosshairInAirFactor +
				CrosshairAimFactor +
				CrosshairShootingFactor;
			
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::ClearHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{

				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;


			HUD->SetHUDPackage(HUDPackage);
		}
	}

}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming && 
		EquippedWeapon->IsMeleeWeapon() == false)
	{
		//if (Character->IsInFirstPerson()) return;

		CurrentFOV = FMath::FInterpTo(
			CurrentFOV, 
			Character->IsInFirstPerson() ? CurrentFOV : EquippedWeapon->GetZoomedFOV(),
			DeltaTime, 
			EquippedWeapon->GetZoomInterpSpeed()
		);
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(
			CurrentFOV, 
			/*Character->IsInFirstPerson() ? CAMERA_POV_FIRST_PERSON_DEFAULT : */DefaultFOV,
			DeltaTime, 
			ZoomInterpSpeed);
	}

	if (Character && Character->GetTPPCamera())
	{
		Character->GetTPPCamera()->SetFieldOfView(CurrentFOV);
	}

}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr) return;
	//not need return when character do not have weapon
	bAiming = bIsAiming;

	ServerSetAiming(bIsAiming);

	/*float WalkSpeed = bRunButtonPressed ? RunSpeed : BaseWalkSpeed;

	Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : WalkSpeed;*/

	if (EquippedWeapon == nullptr) return;

	if (EquippedWeapon->IsMeleeWeapon() == true)
	{
		SetMeleeAiming(bIsAiming); 
	}
	else
	{
		SetRangeAiming(bIsAiming);
	}

}

void UCombatComponent::SetRangeAiming(bool bIsAiming)
{
	if (Character->IsInFirstPerson())
	{
		Character->ADS_Weapon(bIsAiming);
	}

	if (Character->IsLocallyControlled()
		&& EquippedWeapon && EquippedWeapon->IsScopeEquipped())
	{
		Character->ShowSniperScopeWidget(bIsAiming);
	}
}

void UCombatComponent::SetMeleeAiming(bool bIsAiming)
{
	CombatState = bIsAiming ? ECombatState::ECS_Attack : ECombatState::ECS_Unoccupied;
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;

	float WalkSpeed = bRunButtonPressed ? RunSpeed : BaseWalkSpeed;
	if (Character)
	{
		float Speed = bIsAiming ? AimWalkSpeed : WalkSpeed;
		Speed = FMath::Clamp(Speed - GetCharacterMovementReduce(), 0, Speed);

		Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
	}
}

void UCombatComponent::CheckIsPlayerDown(bool IsDown)
{
	bIsDown = IsDown;
}



bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false; //no weapon

	//some condition only valid with non AI Character
	if (Character->IsAICharacter() == false)
	{
		if (CombatState == ECombatState::ECS_Reloading
			&& EquippedWeapon->IsUsingMagazine() == false) return true; //bolt action and shotgun only

		if (CombatState == ECombatState::ECS_Reloading &&
			EquippedWeapon->IsMultiBarrel() &&
			EquippedWeapon->IsAllowedAmmoSwitch() &&
			EquippedWeapon->IsBarrelEmpty())
			return true;
	}
	
	if (EquippedWeapon->IsEmpty()) return false; //empty ammo


	//if (EquippedWeapon->IsBarrelEmpty()) return false; //empty barrel


	if (!bCanFire) return false; //restrict fire
	if (bIsDown) return false; //die or down

	
	if (CombatState != ECombatState::ECS_Unoccupied 
		&& CombatState != ECombatState::ECS_HitReact
		&& CombatState != ECombatState::ECS_Slide
		) return false;
	return true;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast < ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	//this code below will not affect with reload state notify, please check that again
	//code need check again with AI/Onlone Character
	bool bSingleReload = CombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon &&
		EquippedWeapon->IsUsingMagazine() == false && 
		CarriedAmmo == 0;
	if (bSingleReload)
	{
		Character->StopSingleReloadMontage();
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, StartingGrenadeLauncherAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, StartingSRAmmo);
	
}


