#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "CharacterTrajectoryComponent.h"

#include "Components/SpotLightComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/AIModule/Classes/AIController.h"

#include "Engine/SkeletalMeshSocket.h"

//#include "Blaster/BlasterTypes/Constant.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/HUD/OverheadWidget.h"
#include "BlasterAnimInstance.h"
#include "Blaster/Blaster.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/Spawn/SpawnController.h"
#include "Blaster/Unarmed/Unarmed.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/BlasterComponents/BuffComponent.h"
#include "Blaster/BlasterComponents/CharacterPathChapterComponent.h"
#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/RapidFollowPath.h"
#include "Blaster/Path/RangePathType.h"
#include "Blaster/Objects/ScanObjectType.h"
#include "Blaster/Objects/ActorReceiver.h"
#include "Blaster/Path/CalculatePath.h"
#include "Blaster/Objects/ObjectFaceSide.h"
#include "Blaster/Interfaces/BaseFloatingText.h"
#include "Blaster/Weapon/HitboxType.h"
#include "Blaster/PathChapter/PathChapter.h"
#include "Blaster/Objects/Notification.h"
#include "Blaster/Math/MathCalculation.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 220.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocationAndRotation(FVector(-4.f, 0.f, 172.f), FRotator(0.f, 0.f, 0.f));
	CameraBoom->SocketOffset = FVector(0.f, 100.f, 100.f);

	TPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPPCamera"));
	TPPCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TPPCamera->bUsePawnControlRotation = false;



	FPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPPCamera"));
	FPPCamera->SetupAttachment(GetMesh(), FName("head"));
	FPPCamera->bUsePawnControlRotation = false;
	FPPCamera->SetRelativeLocation(FVector(0.f, 20.f, 0.f));
	FPPCamera->SetRelativeRotation(FRotator(0.f, 90.f, -90.f));
	FPPCamera->bUsePawnControlRotation = true;

	CameraADSBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraADSBoom"));
	CameraADSBoom->SetupAttachment(GetMesh(), FName("head"));
	CameraADSBoom->bUsePawnControlRotation = true;
	CameraADSBoom->TargetArmLength = 0.f;

	ADSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ADSCamera"));
	ADSCamera->SetupAttachment(CameraADSBoom);
	ADSCamera->bUsePawnControlRotation = false;
	//ADSCamera->SetRelativeLocation(FVector(-10.f, 80.f, 160.f));
	//ADSCamera->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));
	ADSCamera->SetRelativeScale3D(FVector(.4f));
	ADSCamera->FieldOfView = 50.f;
	//(X=-10.000000,Y=80.000001,Z=160.000000)

	FlashlightTPP = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightTPP"));
	FlashlightTPP->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	FlashlightFPP = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightFPP"));
	FlashlightFPP->SetupAttachment(FPPCamera);


	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	CharacterPathChapter = CreateDefaultSubobject<UCharacterPathChapterComponent>(TEXT("CharacterPathChapter"));
	CharacterPathChapter->SetIsReplicated(true);

	FName LeftMeleeSocket = TEXT("LeftHandSocket");
	FName RightMeleeSocket = TEXT("RightHandSocket");

	LeftMeleeCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftMeleeCollisionBox"));
	LeftMeleeCollisionBox->SetupAttachment(GetMesh(), LeftMeleeSocket);

	RightMeleeCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightMeleeCollisionBox"));
	RightMeleeCollisionBox->SetupAttachment(GetMesh(), RightMeleeSocket);

	CharacterTrajector = CreateDefaultSubobject<UCharacterTrajectoryComponent>(TEXT("CharacterTrajector"));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 350.f);
	GetCharacterMovement()->MaxStepHeight = 50.f;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f);
	GetCharacterMovement()->bUseRVOAvoidance;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 100.f;
	GetCharacterMovement()->AvoidanceWeight = 0.1f;

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);

	GetCharacterMovement()->BrakingFrictionFactor = 0.f;

	//if (LeftMeleeCollisionBox && RightMeleeCollisionBox)
	//{
	//	FAttachmentTransformRules const Rules{
	//		EAttachmentRule::SnapToTarget,
	//		EAttachmentRule::SnapToTarget,
	//		EAttachmentRule::KeepWorld,
	//		false
	//	};
	//	FName LeftMeleeSocket = TEXT("LeftHandSocket");
	//	FName RightMeleeSocket = TEXT("RightHandSocket");
	//	LeftMeleeCollisionBox->AttachToComponent(GetMesh(), Rules, LeftMeleeSocket);
	//	RightMeleeCollisionBox->AttachToComponent(GetMesh(), Rules, RightMeleeSocket);
	//}

}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::Elim_Implementation()
{
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}
	if (Combat)
	{
		if(Combat->EquippedWeapon)
			Combat->EquippedWeapon->Dropped();
		if (Combat->EquippedPrimaryWeapon)
			Combat->EquippedPrimaryWeapon->Dropped();
		if (Combat->EquippedSecondaryWeapon)
			Combat->EquippedSecondaryWeapon->Dropped();
		
		Combat->CheckIsPlayerDown(true);
	}

	bElimmed = true;
	PlayElimMontage();
	RestoreHealth();

	//Disable Character Movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (BlasterPlayerController)
	{
		DisableInput(BlasterPlayerController);
	}

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	bool bHideSniperScope = IsLocallyControlled() && Combat &&
		Combat->bAiming && Combat->EquippedWeapon &&
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (IsLoading() && bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}

	//this only use in mid term project


	// Disable Collision (not use currently because we need to collision to shoot character down)
	/*GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/

	//MulticastElim();
}

//void ABlasterCharacter::MulticastElim_Implementation()
//{
//	bElimmed = true;
//	PlayElimMontage();
//	RestoreHealth();
//}			   

void ABlasterCharacter::AutoPickupEquippedWeapon()
{
	if (Combat)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AI Equip Button Pressed"));
		Combat->PickupWeapon(OverlappingWeapon);
	}

}

void ABlasterCharacter::ReloadAssetData()
{
	AssetPath = USpawnController::GetObjectPath(EScanObjectType::ESOT_Weapon);
}

void ABlasterCharacter::SpawnWeaponAtCharacterLocation()
{
	//if (!HasAuthority()) return;

	////USpawnController* ItemSpawnController = NewObject<USpawnController>();

	//int32 RandWeapon = FMath::RandRange(0, AssetPath.Num() - 1);
	//FString WeaponPath;
	//if (AssetPath.IsValidIndex(RandWeapon))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Asset exist %s"), *FString(AssetPath[RandWeapon]));
	//	WeaponPath = TEXT("/Game/Blueprints/" + AssetPath[RandWeapon]);
	//}

	//USpawnController::SpawnActorAtLocation(WeaponPath, GetWorld(), GetActorLocation(), GetActorRotation());
}


void ABlasterCharacter::SetAICharacterSetup()
{
	bAICharacter = true;
	if (CanEnableRapidFollowPath())
	{
		SetupRapidFollowPath();
	}

}

void ABlasterCharacter::SetAIMovement(float MoveForwardNum, float MoveRightNum)
{
	MoveForward(MoveForwardNum);
	MoveRight(MoveRightNum);
}

void ABlasterCharacter::SetMovementAccelerating(bool bAccelerating)
{
	bMovementAccelerating = bAccelerating;
}
void ABlasterCharacter::SetSightOfPlayer(bool bSightPlayer)
{
	bSightOfPlayer = bSightPlayer;
}

void ABlasterCharacter::SetPathStats()
{
	if (IsSetPathStats) return;
	if (IdlePath)
	{
		if (int32 NoOfPoints = IdlePath->Num())
		{
			int32 MinIndex = 0;
			int32 MaxIndex = NoOfPoints - 1;
			FVector GivenPathPoint;
			PathStats.DefaultZ = IdlePath->GetIdlePoint(0).Z;

			FVector IdlePathLocation = GetIdlePathLocation();

			for (int32 Index = MinIndex; Index <= MaxIndex; Index++)
			{
				GivenPathPoint = IdlePath->GetIdlePoint(Index);

				PathStats.MaxX = FMath::Max(PathStats.MaxX, IdlePathLocation.X + GivenPathPoint.X);
				PathStats.MaxY = FMath::Max(PathStats.MaxY, IdlePathLocation.Y + GivenPathPoint.Y);
				PathStats.MinX = FMath::Min(PathStats.MinX, IdlePathLocation.X + GivenPathPoint.X);
				PathStats.MinY = FMath::Min(PathStats.MinY, IdlePathLocation.Y + GivenPathPoint.Y);
			}

			Box = FVector(
				(double)(PathStats.MaxX + PathStats.MinX) / 2,
				(double)(PathStats.MaxY + PathStats.MinY) / 2,
				PathStats.DefaultZ);
			BoxExtend = FVector(
				(double)(PathStats.MaxX - PathStats.MinX) / 2,
				(double)(PathStats.MaxY - PathStats.MinY) / 2,
				PathStats.DefaultZ);
		}
	}
	//UE_LOG(LogTemp, Error, TEXT("Ans: x[%f | %f]  - y[%f | %f]"), PathStats.MaxX, PathStats.MinX, PathStats.MaxY, PathStats.MinY);
	IsSetPathStats = true;
	PathStats.ShortestDistanceIndex = 5;
}

void ABlasterCharacter::SetLastestPathStats(int32 LastestIndex)
{
	PathStats.CurrentIndex = LastestIndex;
}

void ABlasterCharacter::SetShortestDistanceIndex(int32 ShortestIndex)
{
	PathStats.ShortestDistanceIndex = ShortestIndex;
}


void ABlasterCharacter::ChooseBestPathBox()
{
	FVector CharacterLocation = GetActorLocation();
}

void ABlasterCharacter::OnAttackOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bMeleeTime == false) return;
	if (OtherActor == nullptr || OtherActor->GetOwner() == nullptr) return;
	if (OtherActor->GetOwner() == this->GetOwner()) return;
	if (IsSameTeam(OtherActor->GetOwner())) return;
	if (bMeleeHit == true) return;

	APawn* OwnerPawn = Cast<APawn>(this);
	if (OwnerPawn)
	{
		AController* InstigatorController = OwnerPawn->GetController();
		if (InstigatorController && HasAuthority() && bMeleeFinished == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Doing melee damage 4 "));
			UGameplayStatics::ApplyDamage(
				OtherActor,
				MeleeDamage,
				InstigatorController,
				this,
				UDamageType::StaticClass());

			bMeleeHit = true;
		}
	}
}

void ABlasterCharacter::OnAttackOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{

}


void ABlasterCharacter::SwitchPOV()
{
	SwitchFlashlight();
	bInFirstPerson = !bInFirstPerson;
}

void ABlasterCharacter::SwitchFlashlight()
{
	if (bInFirstPerson)
	{
		FlashlightTPP->SetVisibility(FlashlightFPP->IsVisible());
		FlashlightFPP->SetVisibility(false);
	}
	else
	{
		FlashlightFPP->SetVisibility(FlashlightTPP->IsVisible());
		FlashlightTPP->SetVisibility(false);
	}
}

void ABlasterCharacter::FirstPersonPOV(bool bFirstPersonChange)
{
	bInFirstPerson = bFirstPersonChange;
}

void ABlasterCharacter::FirstPersonAiming()
{
}



void ABlasterCharacter::ADS_Weapon(bool IsAiming)
{
	if (GetEquippedWeapon() == nullptr) return;
}



void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	AAIController* AIController = Cast<AAIController>(this->Controller);

	if (AIController == nullptr)
	{
		UpdateHUDHealth();
		UpdateHUDWeapon();

		FTimerHandle PathChapterTimerHandle;

		//GetWorldTimerManager().SetTimer(PathChapterTimerHandle, this, &ABlasterCharacter::UpdateHUDPathChapter, 2.f, false, 2.f);
		//GetWorldTimerManager().SetTimer(ObjectiveHandle, this, &ABlasterCharacter::SetupNewObjective, 2.f, false, 2.f);
	}

	if (HasAuthority())
	{
		//TODO: (ReloadAssetData) Remove this function into global with const value storage in the memory
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::ReceiveDamage);
	}

	RightMeleeCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABlasterCharacter::OnAttackOverlapBegin);
	RightMeleeCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABlasterCharacter::OnAttackOverlapEnd);
	LeftMeleeCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABlasterCharacter::OnAttackOverlapBegin);
	LeftMeleeCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABlasterCharacter::OnAttackOverlapEnd);



	SetPathStats();

	MeleeEnd();

	UE_LOG(LogTemp, Error, TEXT("Box: %f %f %f"), Box.X, Box.Y, Box.Z);
	//if (IsAICharacter())
	//{
	//	DrawDebugBox(GetWorld(), Box, BoxExtend, FColor::Blue, true, -1, 0, 10);
	//	DrawDebugSphere(GetWorld(), Box, 10.f, 12, FColor::Black, true);
	//}
	//TODO: Path Debug
	CurrentRangePath = ERangePathType::ERPT_NoInRange;

	SetupActorReceiver();

	if (CanEnableRapidFollowPath())
	{
		SetupRapidFollowPath();
	}

	MathCalculation = NewObject<UMathCalculation>(this, UMathCalculation::StaticClass());

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetCharacterCamera(DeltaTime);



	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
	HideCameraIfCharacterClose();
	DownHealthInit();
	PollInit();

	SetRotationRate();
	MeleeOnTick();

	SetFallingTime();

	//if (bInFirstPerson) AimButtonPressed(); 
	//debug for holding aim

	//UE_LOG(LogTemp, Warning, TEXT("bIsInAir is %s"), GetCharacterMovement()->IsFalling() ? TEXT("true") : TEXT("false"));


	//if (!IsAICharacter())
	//{
	//	for (TActorIterator<ABlasterCharacter> Itr(GetWorld()); Itr; ++Itr)
	//	{
	//		if (Itr->GetActorLabel().Contains("test"))
	//		{
	//			//FMath::;
	//			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::SanitizeFloat(GetActorRotation().Yaw));
	//		}
	//	}

	//}



	if (!IsAICharacter())
	{
		/*GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
			FString(
				FString::SanitizeFloat(GetControlRotation().Roll)
			) +
			FString(
				FString::SanitizeFloat(GetControlRotation().Pitch)
			) +
			FString(
				FString::SanitizeFloat(GetControlRotation().Yaw)
			));
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue,
			FString(
				FString::SanitizeFloat(GetActorRotation().Roll)
			) +
			FString(
				FString::SanitizeFloat(GetActorRotation().Pitch)
			) +
			FString(
				FString::SanitizeFloat(GetActorRotation().Yaw)
			));
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow,
			FString(
				FString::SanitizeFloat(GetActorLocation().X)
			) +
			FString(
				FString::SanitizeFloat(GetActorLocation().Y)
			) +
			FString(
				FString::SanitizeFloat(GetActorLocation().Z)
			)
		);*/

		//RapidFollowPath->UpdateAISystem(GetActorLocation(), GetActorQuat());

	}
	//TODO: Change this code from "tick" to "trigger"

	//if (IsAICharacter()) UE_LOG(LogTemp, Warning, TEXT("Save Index = %d"), IdlePath->ShortestDistancePathIndex(GetActorLocation(), PathStats));


}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);

	PlayerInputComponent->BindAction("PrimaryWeapon", IE_Pressed, this, &ABlasterCharacter::PrimaryWeaponButtonPressed);
	PlayerInputComponent->BindAction("SecondaryWeapon", IE_Pressed, this, &ABlasterCharacter::SecondaryWeaponButtonPressed);

	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &ABlasterCharacter::PickupButtonPressed);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &ABlasterCharacter::DropButtonPressed);
	PlayerInputComponent->BindAction("Unequipped", IE_Pressed, this, &ABlasterCharacter::UnequippedButtonPressed);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABlasterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABlasterCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ABlasterCharacter::RunButtonPressed);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ABlasterCharacter::RunButtonReleased);

	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &ABlasterCharacter::MeleeButtonPressed);
	PlayerInputComponent->BindAction("POV", IE_Pressed, this, &ABlasterCharacter::POVButtonPressed);

	PlayerInputComponent->BindAction("Flashlight", IE_Pressed, this, &ABlasterCharacter::FlashlightButtonPressed);

	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &ABlasterCharacter::DebugButtonPressed);


}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;

	}
	if (Buff)
	{
		Buff->Character = this;
	}
	if (CharacterPathChapter)
	{
		CharacterPathChapter->Character = this;
	}
}

void ABlasterCharacter::Falling()
{
	Super::Falling();
	//bFalling = true;
	FallingTimerStart();
}



void ABlasterCharacter::SetRotationRate()
{
	float NewZAxis = 350.f;

	if (GetCharacterMovement()->IsFalling())
	{
		NewZAxis = 50.f;
	}
	else if (bIsCrouched)
	{
		NewZAxis = 200.f;
	}

	GetCharacterMovement()->RotationRate.Yaw = NewZAxis;

}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayMeleeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MeleeMontage)
	{
		FName SectionName;
		if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
		{
			if (bAICharacter) SectionName = FName("HerlobchoMelee");
			else SectionName = FName("Unarmed");
		}
		else
		{
			EWeaponType WeaponType = Combat->EquippedWeapon->GetWeaponType();
			switch (WeaponType)
			{
			case EWeaponType::EWT_AssaultRifle:
			case EWeaponType::EWT_SubmachineGun:
			case EWeaponType::EWT_Shotgun:
			case EWeaponType::EWT_SniperRifle:
				SectionName = FName("Rifle_Punch");
				break;

			case EWeaponType::EWT_Pistol:
				SectionName = FName("Pistol_Whip1");
				break;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("%s Montage"), *SectionName.ToString())
			AnimInstance->Montage_Play(MeleeMontage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	//TODO: Change ReloadMontage at exact weapon name
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("AK_Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("M202_Rocket Reload");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol_Reload");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Kriss_Vector");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("DP-12O Reload");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GL_Reload");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("M49-Nagant");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlaySingleReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
			/*case EWeaponType::EWT_AssaultRifle:
				SectionName = FName("AK_Rifle");
				break;
			case EWeaponType::EWT_RocketLauncher:
				SectionName = FName("M202_Rocket Reload");
				break;
			case EWeaponType::EWT_Pistol:
				SectionName = FName("Colt1911");
				break;
			case EWeaponType::EWT_SubmachineGun:
				SectionName = FName("Kriss_Vector");
				break;*/
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("DP-12O Load");
			break;
			/*case EWeaponType::EWT_GrenadeLauncher:
				SectionName = FName("GLX9");
				break;
			case EWeaponType::EWT_SniperRifle:
				SectionName = FName("M49-Nagant");
				break;*/
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayAmmoSwitchMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AmmoSwitchMontage)
	{
		AnimInstance->Montage_Play(AmmoSwitchMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("AK56");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("M202A16");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("C2911");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Criss_VectorV103");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("DP-12O");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("BoltAction");
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)
	{
		AnimInstance->Montage_Play(ElimMontage);
		AnimInstance->Montage_JumpToSection("Down");
	}

	else if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
		AnimInstance->Montage_JumpToSection("Down_Gun");
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	PlayHitReactMontage(EHitReact::EHR_UP, EHitboxType::EHT_Body);
}

void ABlasterCharacter::PlayHitReactMontage(EHitReact HitReact, EHitboxType HitBoxType)
{
	//if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName;
		switch (HitReact)
		{
		case EHitReact::EHR_UP: SectionName = "HitFront"; break;
		case EHitReact::EHR_DOWN: SectionName = "HitBack"; break;
		case EHitReact::EHR_LEFT: SectionName = "HitLeft"; break;
		case EHitReact::EHR_RIGHT: SectionName = "HitRight"; break;
		}
		switch (HitBoxType)
		{
		case EHitboxType::EHT_Head: SectionName = "HeadShot"; break;
		case EHitboxType::EHT_HandLeft: SectionName = "ArmLeft"; break;
		case EHitboxType::EHT_HandRight: SectionName = "ArmRight"; break;
		case EHitboxType::EHT_Leg:
			SectionName = FName(FString(SectionName.ToString()) + "_Leg");
			break;

		default: break;
		}

		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString(SectionName.ToString()));
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayAttackMontage(bool bAiming)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{

		AnimInstance->Montage_Play(AttackMontage);
		FName SectionName = bAiming ? FName("HeavyAttack") : FName("LightAttack");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayStunMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && StunMontage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, FString("Stun Begin"));
		AnimInstance->Montage_Play(StunMontage);
		FName SectionName = FName("Weapon_Melee");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}


void ABlasterCharacter::TriggerPathChapter(FString CollisionName)
{
	if (CharacterPathChapter)
	{
		CharacterPathChapter->TriggerPathChapter(CollisionName);
	}
}

void ABlasterCharacter::PickUpObjectiveItem()
{
	if (CharacterPathChapter)
	{
		CharacterPathChapter->PickUpObjectiveItem();
	}
}

void ABlasterCharacter::AddToKillWithObjective(AActor* DamageCharacter)
{

}


void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (bEnableInvisible) return;

	/*
	* some weapon does not require to damage yourself
	* Condition 1: test if weapon damage yourself, if true next condition
	* Condition 2: If weapon is the type for radius weapon damage 
	* (Rocket, Grenade)
	*/
	if (IsDamageYourself(DamageCauser) &&
		IsWeaponCanDamageYourself(DamageCauser) == false) return;

	//UE_LOG(LogTemp, Warning, TEXT("%s damage"), *this->GetName());

	//this line is added for reduce damage for player
	//this still in development 
	float DamageAIReduceToPlayer = 100.f;

	if (IsAICharacter() == false) //player received damage
	{
		DamageAIReduceToPlayer = 20.f;
	}
	Damage = Damage / 100 * DamageAIReduceToPlayer;

	//apply damage to health
	Health = FMath::Clamp((Health - Damage), 
		0.f,
		MaxHealth);
	bGettingHit = true;

	UpdateHealthBar();

	if (bAICharacter == false)
	{
		ShowDamageScreen(DamageCauser, Damage);
		UpdateHUDHealth();
	}

	TargetGettingHit = DamageCauser->GetActorLocation();

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue,
	//	/*FString("Damage Actor: " +
	//		FString::SanitizeFloat(DamagedActor->GetActorRotation().Pitch)
	//	) + " " + */
	//	FString(
	//		FString::SanitizeFloat(DamagedActor->GetActorRotation().Yaw)
	//	));;
	//


	if (Health == 0.f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Player down..."));
		//if (bElimmed)
		//{
		//	bDeath = true;
		//	//UE_LOG(LogTemp, Warning, TEXT("Cant have any health left!"));

		//	//detroy actor after death
		//}
		//else 
		//{
		if (ABlasterGameMode* CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>())
		{
			BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
			ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);

			UE_LOG(LogTemp, Error, TEXT("Damage by = %s"), *DamageCauser->GetName());


			CharacterGameMode->PlayerEliminated(
				this,
				BlasterPlayerController,
				AttackerController);
			bElimmed = true;
			bDeath = true; //temporary
			if (Combat)
			{
				Combat->DropEquippedWeapon();
			}

			if (AttackerController)
			{
				ABlasterCharacter* AttackerCharacter = Cast<ABlasterCharacter>(AttackerController->GetPawn());
				if (AttackerCharacter && !AttackerCharacter->IsAICharacter())
				{
					AttackerCharacter->AddToKillWithObjective(DamagedActor);
				}
			}
			FTimerHandle DestroyHandle;
			GetWorldTimerManager().SetTimer(DestroyHandle, this, &ABlasterCharacter::DestroyCharacter, 5.f, false);
		}
		//}		
	}
	else
	{
		ABlasterPlayerController* AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
		if (AttackerController)
		{
			ABlasterCharacter* AttackerCharacter = Cast<ABlasterCharacter>(AttackerController->GetPawn());
			if (AttackerCharacter)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
				//	//FString("Damage Causer: " +
				//	//	FString::SanitizeFloat(DamageCauser->GetActorRotation().Pitch)
				//	//) + " " +
				//	FString(
				//		FString::SanitizeFloat(AttackerCharacter->GetActorRotation().Yaw)
				//	));;

				float HitReactYaw = DamagedActor->GetActorRotation().Yaw - AttackerCharacter->GetActorRotation().Yaw;

				if (Combat)
				{
					Combat->SetHitReactState();
				}
				EHitReact HitReact = GetHitReactYaw(HitReactYaw);
				//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString::SanitizeFloat(HitReactYaw));
				//TODO: Yaw to check which rotate from attacker to target 

				if (AttackerCharacter->GetEquippedWeapon())
				{
					PlayHitReactMontage(HitReact, AttackerCharacter->GetEquippedWeapon()->GetHitboxType());

					if (AttackerCharacter->GetEquippedWeapon()->IsEnableStun())
					{
						PlayStunMontage();
					}
				}
			}
		}

		//this only valid with AI Character
		if (bAICharacter)
		{
			AAICharacterController* AIDamageController = Cast<AAICharacterController>(this->GetController());
			AActor* CharacterCauser = Cast<AActor>(InstigatorController->GetPawn());
			if (AIDamageController && CharacterCauser) 
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, FString(CharacterCauser->GetName()));
				if (IsSameTeam(CharacterCauser) == false)
				{
					AIDamageController->ReportDamageInBlueprint(DamagedActor, Damage, DamageCauser);
				}		
			}
		}
	}
	/*
	if(bElimmed && !bDeath)
	{
		Health = FMath::Clamp(Health - DownHealthDelay - Damage, 0.f, MaxHealth);
		UpdateHUDHealth();
		//GetWorldTimerManager().SetTimer(
		//	DownHealthTimer,
		//	this,
		//	&ABlasterCharacter::DownHealthFinished,
		//	DownHealthDelay
		//);
		if (Health == 0.f)
		{
		}
	}
	*/
}

void ABlasterCharacter::DisableGettingHit()
{
	bGettingHit = false;
}

void ABlasterCharacter::DestroyCharacter()
{
	//if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLabel().Contains(this->GetActorLabel())) return;
	ABlasterGameMode* CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (CharacterGameMode)
	{
		Destroy();
		CharacterGameMode->DecreaseCharacterNum();
	}

}

void ABlasterCharacter::StopReloadMontage()
{
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Pause(ReloadMontage);
		GetMesh()->GetAnimInstance()->Montage_Stop(0.f, ReloadMontage);
	}

}

void ABlasterCharacter::StopSingleReloadMontage()
{
	//rocket ammo will change some line code later
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
			/*case EWeaponType::EWT_AssaultRifle:
				SectionName = FName("AK_Rifle");
				break;
			case EWeaponType::EWT_RocketLauncher:
				SectionName = FName("M202_Rocket Reload");
				break;
			case EWeaponType::EWT_Pistol:
				SectionName = FName("Colt1911");
				break;
			case EWeaponType::EWT_SubmachineGun:
				SectionName = FName("Kriss_Vector");
				break;*/
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("M202_Rocket End Reload");
			break;
		case EWeaponType::EWT_Shotgun: //only montage can be work
			SectionName = FName("DP-12O End Reload");
			break;
			/*case EWeaponType::EWT_GrenadeLauncher:
				SectionName = FName("GLX9");
				break;
			case EWeaponType::EWT_SniperRifle:
				SectionName = FName("M49-Nagant");
				break;*/
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::StopAmmoSwitchMontage()
{
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AmmoSwitchMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Pause(AmmoSwitchMontage);
		GetMesh()->GetAnimInstance()->Montage_Stop(0.f, AmmoSwitchMontage);
	}
}

void ABlasterCharacter::StopAttackMontage()
{
	if (Combat == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Pause(AttackMontage);
		GetMesh()->GetAnimInstance()->Montage_Stop(0.f, AttackMontage);
	}
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
}

void ABlasterCharacter::MoveForward(float Value)
{
	if (Value == 0)
	{
		bForwardPressed = false;
		CheckRunForwardRelease();
		
	}
	else
	{
		bForwardPressed = true;
		CheckRunForwardPressed();
		
	}
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}

}

void ABlasterCharacter::MoveRight(float Value)
{

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));


		AddMovementInput(Direction, Value);
	}

}

void ABlasterCharacter::Turn(float Value)
{
	YawInput = Value;
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value)
{
	PitchInput = Value;
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::PrimaryWeaponButtonPressed()
{
	if (HasAuthority())
	{
		Combat->EquipPrimaryWeapon();
	}
	else
	{
		ServerEquipButtonPressed();
	}

	UpdateHUDWeapon();
}

void ABlasterCharacter::SecondaryWeaponButtonPressed()
{
	if (HasAuthority())
	{
		Combat->EquipSecondaryWeapon();
	}
	else
	{
		ServerEquipButtonPressed();
	}

	UpdateHUDWeapon();
}

void ABlasterCharacter::MeleeWeaponButtonPressed()
{
}

void ABlasterCharacter::PickupButtonPressed()
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->PickupWeapon(OverlappingWeapon);
		}
		else
		{
			ServerPickupButtonPressed();
		}

		UpdateHUDWeapon();
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	//this code is unavailable, so this mean code wont work anymore
	//if (Combat)
	//{
	//	if (HasAuthority())
	//	{			
	//		Combat->EquipWeapon(OverlappingWeapon);
	//	}
	//	else
	//	{
	//		ServerEquipButtonPressed();
	//	}

	//	UpdateHUDWeapon();
	//}
}

void ABlasterCharacter::DropButtonPressed()
{
	if (Combat)
	{
		Combat->DropEquippedWeapon();
	}
	UpdateHUDWeapon();
}

void ABlasterCharacter::ServerPickupButtonPressed_Implementation()
{
	if (Combat)
	{
		//Combat->(OverlappingWeapon);
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		//Combat->(OverlappingWeapon);
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		if (bEnableSlide)
		{
			Slide();
		}
		else Crouch();
	}
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if (Combat)
	{
		Combat->Reload();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->SetAiming(true);
	}
	if (IsWeaponEquipped())
	{
		bAimingState = true; //ADS with weapon only
	}
	else bCharacterSight = true; //character Sight only

}


void ABlasterCharacter::AimButtonReleased()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->SetAiming(false);
	}
	bCharacterSight = false;
	bAimingState = false;
}

void ABlasterCharacter::RunButtonPressed()
{
	bRunPressed = true;
	if (Combat)
	{
		Combat->RunButtonPressed(true);
	}

}

void ABlasterCharacter::RunButtonReleased()
{
	bRunPressed = false;
	if (Combat)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Begin End of the Running"));
		Combat->RunButtonPressed(false);
	}
	bEnableSlide = true;
}


void ABlasterCharacter::MeleeButtonPressed()
{
	Melee();
}

void ABlasterCharacter::POVButtonPressed()
{
	SwitchPOV();
}

void ABlasterCharacter::UnequippedButtonPressed()
{
	if (Combat)
	{
		Combat->UnequippedWeapon();
	}
	UpdateHUDWeapon();
}

void ABlasterCharacter::FlashlightButtonPressed()
{
	if (bInFirstPerson)
	{
		FlashlightFPP->SetVisibility(!FlashlightFPP->IsVisible());
	}
	else
	{
		FlashlightTPP->SetVisibility(!FlashlightTPP->IsVisible());
	}
	
}

void ABlasterCharacter::DebugButtonPressed()
{

	MoveForward(-10.f);

#if 0
	int32 Count = 0;

	float MaxRange = 1000.f;

	//DrawDebugSphere(GetWorld(), GetActorLocation(), 12.f, 12, FColor::Red, true);

	for (TObjectIterator<AActor> Itr; Itr; ++Itr)
	{
		float Distance = GetDistanceTo(*Itr);
		//DrawDebugLine(GetWorld(), GetActorLocation(), Itr->GetActorLocation(), FColor::White, true);
		//
		if (Distance < MaxRange)
		{
			//DrawDebugLine(GetWorld(), GetActorLocation(), Itr->GetActorLocation(), FColor::White, true);
			//TODO: Object In Range Debug
			Count++;
			//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan,
			//	Itr->GetClass()->GetName()
			//);
		}
	}
	//DrawDebugSphere(GetWorld(), GetActorLocation(), MaxRange, 6, FColor::Red, true, 2.f);



	//TArray<FHitResult>AvailableObjects;
	//FVector ActorRangeSize = FVector(
	//	GetActorLocation().X + MaxRange,
	//	GetActorLocation().Y + MaxRange,
	//	GetActorLocation().Z + MaxRange);

	//FCollisionShape Shape = FCollisionShape::MakeBox(FVector(MaxRange, MaxRange, MaxRange));
	//GetWorld()->SweepMultiByObjectType(AvailableObjects,
	//	GetActorLocation(),
	//	ActorRangeSize,
	//	FQuat(),
	//	FCollisionObjectQueryParams::AllObjects,
	//	Shape);
	//UE_LOG(LogTemp, Warning, TEXT("Array: %d | %d"), Count, AvailableObjects.Num());



	//DrawDebugSphere(GetWorld(), ResultLocation.LocationA, 12.f, 12, FColor::Orange, true);
//DrawDebugString(GetWorld(), ResultLocation.LocationA, "LocA");
//DrawDebugSphere(GetWorld(), ResultLocation.LocationB, 12.f, 12, FColor::Orange, true);
//DrawDebugString(GetWorld(), ResultLocation.LocationA, "LocB");

//DrawDebugBox(GetWorld(), Origin, ItrExtent, FColor::Yellow, true, -1, 0, 10);
//Itr->Single
//DrawDebugSphere(GetWorld(), ShortestObjectLocation, 5.f, 12, FColor::Emerald, true);

//DrawDebugLine(GetWorld(), 
//	GetActorLocation(), 
//	ShortestObjectLocation,
//	FColor::White, 
//	true);

#endif

	//UObjectFaceSide* ObjectFaceSide = NewObject<UObjectFaceSide>(this, UObjectFaceSide::StaticClass());

	//int32 Count = 0;
	//float MaxRange = 2000.f;
	//FVector TestLoc = FVector(3610.f, -4580.f, 0.f);

	//CalculatePath = NewObject<UCalculatePath>();

	//for (TObjectIterator<AActor> Itr; Itr /*&& count <= 5*/; ++Itr)
	//{
	//	float Distance = GetDistanceTo(*Itr);

	//	FVector ShortestObjectLocation;
	//	float ShortestDistance = Itr->ActorGetDistanceToCollision(GetActorLocation(), ECollisionChannel::ECC_Visibility, ShortestObjectLocation);
	//	if (//*Itr->GetActorLabel() == FString("Cube207") ||
	//		//*Itr->GetActorLabel() == FString("triangle2") ||
	//		true)
	//	{
	//		if (Distance > 0 && Distance <= MaxRange
	//			|| ShortestDistance > 0 && ShortestDistance <= MaxRange)
	//		{
	//			
	//			ObjectFaceSide->PreloadObject(*Itr, GetActorLocation(), ShortestObjectLocation, GetActorRotation(), Itr->GetActorRotation());

	//			Count = 1;
	//			UE_LOG(LogTemp, Warning, TEXT("%s: %f"), *Itr->GetName(), ShortestDistance);
	//		}
	//	}
	//}
	//ObjectFaceSide->AutoPredictPossibleMoveFromLatestData();
	//ObjectFaceSide->Clear();




}

void ABlasterCharacter::Melee() //_Implementation
{
	if (bMeleeFinished == true)
	{
		bMeleeFinished = false;
		bMeleeHit = false;

		if (Combat)
		{
			if (Combat->EquippedWeapon && !IsRestrictWeapon())
			{
				Combat->Melee();
			}
			else UnarmedMelee();
		}
		else
		{
			UnarmedMelee();
		}
		//bMeleeFinished = false;
	}
}

void ABlasterCharacter::MeleeStart()
{
	RightMeleeCollisionBox->SetCollisionProfileName("Fist");
	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(true);
	LeftMeleeCollisionBox->SetCollisionProfileName("Fist");
	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(true);
	bMeleeTime = true;
	//there will be another way, but we will find this issue later
}

void ABlasterCharacter::MeleeEnd()
{
	RightMeleeCollisionBox->SetCollisionProfileName("Fist");
	RightMeleeCollisionBox->SetNotifyRigidBodyCollision(false);
	LeftMeleeCollisionBox->SetCollisionProfileName("Fist");
	LeftMeleeCollisionBox->SetNotifyRigidBodyCollision(false);
	bMeleeTime = false;
}

void ABlasterCharacter::MeleeOnTick()
{
	if (bMeleeFinished == false)
	{
		if (IsMontageFinished(MeleeMontage))
		{
			bMeleeFinished = true;
			bMeleeHit = false;
		}
	}
}

void ABlasterCharacter::StopAttack()
{
	if (Combat)
	{
		Combat->FinishAttack();
		StopAttackMontage();
	}

}

void ABlasterCharacter::UnarmedMelee()
{
	GetCharacterMovement()->DisableMovement();
	PlayMeleeMontage();
}

void ABlasterCharacter::SetupActorReceiver()
{
	ActorReceiver = NewObject<UActorReceiver>(this, UActorReceiver::StaticClass());
	ActorReceiver->SetupActorReceiver();
}


float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	//if (bInFirstPerson) return;
	float Speed = CalculateSpeed();
	bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) //standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) //running, or jumping
	{

		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{
	//TODO: sometimes AO_Pitch return to reverse position of character
	AO_Pitch = GetBaseAimRotation().Pitch;

	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);

	}
	/*else if (AO_Pitch < -90.f && !IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Out of range"));

	}*/
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > STAND_SPEED)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	//UE_LOG(LogTemp, Warning, TEXT("ProxyYaw: %f"), ProxyYaw);

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		//bUseControllerRotationYaw = true;
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::Jump()
{
	if (bSlideState)
	{
		StopSlide(); //disable instantly for jumping
	}
	
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::FallingTimerStart()
{
	float DefaultFallingTime = DEFAULT_FALLING_TIMER;

	GetWorldTimerManager().ClearTimer(JumpFallingTimer);
	GetWorldTimerManager().SetTimer(
		JumpFallingTimer,
		DefaultFallingTime,
		false);
}

void ABlasterCharacter::SetFallingTime()
{
	FallingTime =
		GetWorldTimerManager().GetTimerRemaining(JumpFallingTimer);
	//if(!IsAICharacter())
	//	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString::SanitizeFloat(
	//		GetWorldTimerManager().GetTimerRemaining(JumpFallingTimer)
	//	));
}

void ABlasterCharacter::Slide()
{
	if (Combat)
	{
		GetWorldTimerManager().ClearTimer(RunHandle);

		Combat->SetSlideState();

		bSlideState = true;
		bRunState = true;
		if (GetWorldTimerManager().IsTimerActive(SlideHandle) == false)
		{
			GetWorldTimerManager().SetTimer(SlideHandle, this, &ABlasterCharacter::StopSlide, DEFAULT_TIMER_SLIDE, false);

			GetWorldTimerManager().SetTimer(SlideInterruptHandle, this, &ABlasterCharacter::StopSlideStateInterrupt, DEFAULT_TIMER_SLIDE, false);
		}
	}
}

void ABlasterCharacter::StopSlide()
{
	bEnableSlide = false;
	bSlideState = false;
	bRunState = false;

}

void ABlasterCharacter::StopSlideStateInterrupt()
{
	if (Combat)
	{
		Combat->FinishSlide();
		//some cases we using fast for skip slide, 
		// so we will using it as penalty
	}
}

void ABlasterCharacter::CheckRunForwardPressed()
{
	if (bCheckRunForwardPressed)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString("bCheckRunForwardPressed"));
		/*GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, FString("bRunPressed = " +
			bRunPressed ? FString("true") : FString("false")));*/
		bCheckRunForwardPressed = !bCheckRunForwardPressed;
		if (Combat && bRunPressed)
		{
			Combat->RunButtonPressed(true);
		}
	}
}

void ABlasterCharacter::CheckRunForwardRelease()
{
	if (bCheckRunForwardPressed == false)
	{
		bCheckRunForwardPressed = !bCheckRunForwardPressed;
		if (Combat)
		{
			Combat->RunButtonPressed(false);
		}
	}
}

void ABlasterCharacter::SetRunningTimer()
{
	GetWorldTimerManager().ClearTimer(RunHandle);
	GetWorldTimerManager().SetTimer(RunHandle, this, &ABlasterCharacter::StopRunning, DEFAULT_TIMER_RUN, false);
}

void ABlasterCharacter::StopRunning()
{
	if (bRunState == false) StopSlide();
}



void ABlasterCharacter::AIFire(bool bFireActive)
{
	if (GetEquippedWeapon() && GetEquippedWeapon()->IsEmpty())
	{
		AIReload();
		return;
	}
	bFireActive ? FireButtonPressed() : FireButtonReleased();
}

void ABlasterCharacter::AIReload()
{
	ReloadButtonPressed();
}

void ABlasterCharacter::FireButtonPressed()
{
	if (IsWeaponEquipped())
	{
		Combat->FireButtonPressed(true);
	}
	else
	{
		MeleeButtonPressed();
	}
}

void ABlasterCharacter::FireButtonReleased()
{
	if (Combat) {
		Combat->FireButtonPressed(false);
	}
}



void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > MAX_TURNINPLACE_RIGHT)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < MAX_TURNINPLACE_LEFT)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterAO_Yaw = FMath::FInterpTo(InterAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if (bInFirstPerson) return;
	if ((TPPCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ABlasterCharacter::ShowHUDEndObjective()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDEndObjective();
	}
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::UpdateHUDWeapon()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeapon(GetEquippedWeapon());
		BlasterPlayerController->SetHUDBarrel(GetEquippedWeapon());

		//FTimerHandle HUDTimer;
		//GetWorldTimerManager().SetTimer(HUDTimer, this, &ABlasterCharacter::UpdateHUDWeaponBarrel, 1.f, false);
	}
}

void ABlasterCharacter::UpdateHUDWeaponBarrel()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDBarrel(GetEquippedWeapon()); //Add Delay
	}

}

void ABlasterCharacter::UpdateHUDPathChapter()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		if (CharacterPathChapter && CharacterPathChapter->PathChapter)
		{
			BlasterPlayerController->SetHUDPathChapter(CharacterPathChapter->PathChapter->GetPathChapterDetail());
		}
	}
}

void ABlasterCharacter::UpdateHUDObjective()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		if (CharacterPathChapter && CharacterPathChapter->PathChapter)
		{
			BlasterPlayerController->SetHUDObjective(CharacterPathChapter->PathChapter->GetPathChapterDetail());
		}
	}
}

void ABlasterCharacter::UpdateHUDCaptureTimer()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		if (CharacterPathChapter)
		{
			BlasterPlayerController->SetHUDCaptureTimer(CharacterPathChapter->GetCharacterCaptureTimer());
		}
	}
}

void ABlasterCharacter::UpdateHUDNumRequired()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		if (CharacterPathChapter)
		{
			BlasterPlayerController->SetHUDNumRequired(CharacterPathChapter->GetNoObjectiveRemain());
		}	
	}
}

void ABlasterCharacter::DisableHUDCaptureTimer()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->DisableHUDCaptureTimer();
	}

}

void ABlasterCharacter::ShowDamageScreen(AActor* DamageCauser, float Damage)
{
	ABlasterCharacter* CharacterCauser = Cast<ABlasterCharacter>(DamageCauser->GetOwner());

	if (CharacterCauser && !CharacterCauser->IsAICharacter())
	{
		if (CharacterCauser->GetEquippedWeapon())
		{
			EHitboxType CurrentHitbox = CharacterCauser->GetEquippedWeapon()->GetHitboxType();
			FVector TextLocation = CharacterCauser->GetHitTarget();

			FRotator TextRotation(
				DamageCauser->GetActorQuat().X,
				DamageCauser->GetActorQuat().Y + 180.f,
				DamageCauser->GetActorQuat().Z);
			FTransform DamageTextTransform;
			DamageTextTransform.SetLocation(TextLocation);
			DamageTextTransform.SetRotation(TextRotation.Quaternion());

			ABaseFloatingText* DamageReceiveText = GetWorld()->SpawnActorDeferred<ABaseFloatingText>(
				ABaseFloatingText::StaticClass(),
				DamageTextTransform,
				CharacterCauser->GetOwner()
			);
			if (DamageReceiveText)
			{
				DamageReceiveText->SetDamageText(Damage);
				DamageReceiveText->SetTextColor(CurrentHitbox);
				DamageReceiveText->SetCauserCharacter(DamageCauser);

				UGameplayStatics::FinishSpawningActor(DamageReceiveText, DamageTextTransform);
			}

		}
	}

}

void ABlasterCharacter::RestoreHealth()
{
	if (bElimmed && !bDeath)
	{
		Health = MaxHealth;
		UpdateHUDHealth();
	}
	//TODO: restore health can be use while using items or down
}

void ABlasterCharacter::DownHealthInit()
{
	if (bElimmed && !bDeath)
	{
		Health = FMath::Clamp(Health - DownHealthDelay, MIN_HEALTH, MaxHealth);
		UpdateHUDHealth();
		if (Health == MIN_HEALTH)
		{
			bDeath = true;
		}
	}
	//GetWorldTimerManager().SetTimer(
	//	DownHealthTimer,
	//	this,
	//	&ABlasterCharacter::DownHealthFinished,
	//	DownHealthDelay
	//);
		//delay...but it error when run
}

void ABlasterCharacter::PollInit()
{
	if (BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if (BlasterPlayerState)
		{
			BlasterPlayerState->AddToKills(0.f);
			CharacterPathChapter->BlasterPlayerState = BlasterPlayerState;
		}
	}
}




//void ABlasterCharacter::DownHealthFinished()
//{
//	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
//	
//	if (BlasterGameMode)
//	{
//		Health -= 1;
//		UpdateHUDHealth();
//	}
//}

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void ABlasterCharacter::SetCharacterCamera(float DeltaTime)
{
	if (bInFirstPerson)
	{
		SetCharacterCameraFirstPerson(DeltaTime);
		return;
	}
	FPPCamera->SetActive(false);
	TPPCamera->SetActive(true);
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Done TPP"));
	FVector NewCameraLocation = CharacterThirdPerson.CameraBoomLocation;
	float NewTargetArmLength = 220.f;
	FVector NewSocketOffset = FVector(0.f, 100.f, 0.f);


	//default: unequipped, hip-fire(equipped)

	if (bCharacterSight)
	{
		NewTargetArmLength = FMath::FInterpTo(
			CameraBoom->TargetArmLength,
			DEFAULT_THIRD_PERSON_CHARACTER_POV_SIGHT,
			DeltaTime,
			5.f);

		//Character Sight (when Aim Pressed Button without weapon equipped)
		//Character Sight will use only with Third Person Mode
	}

	else
	{
		if (Combat && Combat->EquippedWeapon && Combat->bAiming)
		{
			NewTargetArmLength = FMath::FInterpTo(
				CameraBoom->TargetArmLength,
				180.f,
				DeltaTime,
				20.f);  //TODO: Apply Speed not hard code when character aiming
		}
		//aiming (ADS)

		else
		{
			NewTargetArmLength = FMath::FInterpTo(
				CameraBoom->TargetArmLength,
				CharacterThirdPerson.TargetAimLength - (bRunState ? 100.f : 0.f),
				DeltaTime,
				5.f);
			//no aiming / default TPP 
		}
	}

	if (bIsCrouched)
	{
		NewCameraLocation = FMath::VInterpTo(
			CameraBoom->GetRelativeLocation(),
			CharacterThirdPersonCrouch.CameraBoomLocation,//CharacterThirdPersonCrouch.CameraBoomLocation, //default: FVector(-10.f, 60.f, 60.f)
			DeltaTime,
			5.f);
		NewSocketOffset = FMath::VInterpTo(
			CameraBoom->SocketOffset,
			CharacterThirdPersonCrouch.CameraBoomSocket, //default: FVector(-10.f, 60.f, 60.f)
			DeltaTime,
			5.f);
	}
	else
	{
		NewCameraLocation = FMath::VInterpTo(
			CameraBoom->GetRelativeLocation(),
			CharacterThirdPerson.CameraBoomLocation, //default: FVector(-10.f, 60.f, 60.f)
			DeltaTime,
			20.f);
		NewSocketOffset = FMath::VInterpTo(
			CameraBoom->SocketOffset,
			CharacterThirdPerson.CameraBoomSocket,  //default: FVector(0.f, 70.f, 100.f)
			DeltaTime,
			20.f);
	}
	//crouching


	CameraBoom->SetRelativeLocation(NewCameraLocation);
	CameraBoom->TargetArmLength = NewTargetArmLength;
	CameraBoom->SocketOffset = NewSocketOffset;

}

void ABlasterCharacter::SetCharacterCameraFirstPerson(float DeltaTime)
{
	//TPPCamera->SetFieldOfView(90.f);


	float NewTargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		CharacterFirstPerson.TargetAimLength,
		DeltaTime,
		20.f);

	FVector NewSocketOffset = FMath::VInterpTo(
		CameraBoom->SocketOffset,
		CharacterFirstPerson.CameraBoomSocket,
		DeltaTime,
		20.f);

	FVector NewCameraLocation = FMath::VInterpTo(
		CameraBoom->GetRelativeLocation(),
		FVector(0.f, 20.f, 160.f),
		DeltaTime,
		20.f);

	if (CameraBoom->TargetArmLength == NewTargetArmLength)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Black, FString("Done FPP"));
		FPPCamera->SetActive(true);
		TPPCamera->SetActive(false);
		if (Combat->bAiming == false) ADSCamera->SetActive(false);
	}

	if (Combat && Combat->EquippedWeapon && Combat->bAiming)
	{
		FVector ADSWeaponLocation = FVector();
		FTransform SightTransform = GetEquippedWeapon()->GetWeaponAimSightTransform(ERelativeTransformSpace::RTS_World);

		//FRotator SightRotation = GetEquippedWeapon()->GetWeaponAimSightTransform(ERelativeTransformSpace::RTS_ParentBoneSpace).GetRotation().Rotator();

		/*FVector SightLocation = SightTransform.GetLocation();
		FTransform ADSWeaponTransform = FTransform(
			SightTransform.GetRotation().Rotator(),
			 SightLocation,
			SightTransform.GetScale3D());
		ADSCamera->SetWorldTransform(ADSWeaponTransform);*/

		//ADSCamera->AttachToComponent(GetEquippedWeapon()->GetWeaponMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), FName("ADS_Socket"));
		//CameraADSBoom->TargetArmLength = UMathCalculation::GetDistance(FPPCamera->GetComponentLocation(), SightTransform.GetLocation());
		CameraADSBoom->SetWorldTransform(SightTransform);

		CameraADSBoom->TargetOffset.Z = SightTransform.GetRotation().Vector().Z;


		FPPCamera->SetActive(false);
		ADSCamera->SetActive(true);


	}
	//aiming (ADS)

	CameraBoom->SetRelativeLocation(NewCameraLocation);
	CameraBoom->TargetArmLength = NewTargetArmLength;
	CameraBoom->SocketOffset = NewSocketOffset;

}

void ABlasterCharacter::SetCharacterRunState(bool IsRunState)
{
	bRunState = IsRunState;
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled() &&
		!IsAICharacter())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	//UE_LOG(LogTemp, Warning, TEXT("Overlapping Weapon!"));
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}
//
bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

bool ABlasterCharacter::IsMontageFinished(UAnimMontage* Montage)
{
	return GetMesh()->GetAnimInstance()->Montage_GetIsStopped(Montage);
}

bool ABlasterCharacter::IsMontageRunning()
{
	return false;
}

bool ABlasterCharacter::IsRestrictWeapon()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return true;
	return Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_RocketLauncher;
	//TODO: list all restrict weapon right there
}

bool ABlasterCharacter::IsInRangePath()
{
	FVector GivenCharacterLocation = GetActorLocation();
	if (GivenCharacterLocation.X <= PathStats.MaxX && GivenCharacterLocation.X >= PathStats.MinX)
	{
		CurrentRangePath = ERangePathType::ERPT_InRangeX;
	}
	if (GivenCharacterLocation.Y <= PathStats.MaxY && GivenCharacterLocation.Y >= PathStats.MinY)
	{
		if (CurrentRangePath == ERangePathType::ERPT_InRangeX)
		{
			CurrentRangePath = ERangePathType::ERPT_InRangeXY;
		}
		else
		{
			CurrentRangePath = ERangePathType::ERPT_InRangeY;
		}
	}
	PathStats.CurrentLocationRange = CurrentRangePath;

	if (CurrentRangePath != ERangePathType::ERPT_NoInRange)
	{
		return true;
	}
	return false;
}

bool ABlasterCharacter::CanEnableRapidFollowPath()
{
	return AllowRapidFollowPath;
}

bool ABlasterCharacter::IsWeaponDrop()
{
	return false;
}

bool ABlasterCharacter::IsFPPAiming()
{
	return IsInFirstPerson() && IsAiming();
}

bool ABlasterCharacter::IsInCollectObjective()
{
	if (CharacterPathChapter)
	{
		return CharacterPathChapter->IsInCollectObjective();
	}
	return false;
}

bool ABlasterCharacter::IsDamageYourself(AActor* DamageCauser)
{
	return this == DamageCauser->GetOwner();
}

bool ABlasterCharacter::IsWeaponCanDamageYourself(AActor* DamageCauser)
{
	if (IsWeaponEquipped())
	{
		if (GetEquippedWeapon()->IsProjectileDamageRadius()) return true;

		EWeaponType WeaponType = GetEquippedWeapon()->GetWeaponType();
		return (WeaponType == EWeaponType::EWT_GrenadeLauncher ||
			WeaponType == EWeaponType::EWT_RocketLauncher);
	}
	return false;
}

bool ABlasterCharacter::IsSameTeam(AActor* TargetActor)
{
	return TargetActor->GetName().Contains(this->GetName());
}

bool ABlasterCharacter::IsValidTargetCharacterInSight(AActor* TargetActor)
{
	if (this == nullptr || TargetActor == nullptr) return false;
	if (TargetActor == this->GetOwner()) return false;

	if((TargetActor->GetActorLocation() - this->GetActorLocation()).Size() > NEAR_PLAYER_DISTANCE) 
		return false;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(TargetActor);
	QueryParams.AddIgnoredActor(this);
	bool InValidSight = GetWorld()->LineTraceSingleByChannel(
		FirstTargetSight,
		this->GetActorLocation(),
		TargetActor->GetActorLocation(),
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);

	if (!InValidSight) return true;

	//if(FirstTargetSight.GetActor() == nullptr) return true;

	return false;
}

bool ABlasterCharacter::IsNearPlayer()
{
	if (PlayerPawn == this) return false;
	return (PlayerPawn->GetActorLocation() - this->GetActorLocation()).Size() <= NEAR_PLAYER_DISTANCE;
}

EHitReact ABlasterCharacter::GetHitReactYaw(float HitReactYaw)
{
	if (45.f <= HitReactYaw && HitReactYaw < 135.f) return EHitReact::EHR_RIGHT;
	if (135.f <= HitReactYaw && HitReactYaw < 225.f) return EHitReact::EHR_UP;
	if (225.f <= HitReactYaw && HitReactYaw < 315.f) return EHitReact::EHR_LEFT;
	//else
	return EHitReact::EHR_DOWN;
}

void ABlasterCharacter::ResetCurrentRangePath()
{
	CurrentRangePath = ERangePathType::ERPT_NoInRange;
}

void ABlasterCharacter::SetupRapidFollowPath()
{
	RapidFollowPath = NewObject<URapidFollowPath>(this, URapidFollowPath::StaticClass());
	//RapidFollowPath->SetupAISystem();
}

void ABlasterCharacter::SetDetectedState(bool DetectedState)
{
	bDetectedCharacter = DetectedState;
}

UCombatComponent* ABlasterCharacter::GetCombat()
{
	return Combat;
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}


ECombatState ABlasterCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

AIdlePath* ABlasterCharacter::GetIdlePath() const
{
	return IdlePath;
}

int32* ABlasterCharacter::GetIndexCharacter() const
{
	return 0;
}

FVector ABlasterCharacter::GetLatestPathStats() const
{
	if (IdlePath == nullptr) return FVector();
	return IdlePath->GetIdlePoint(PathStats.CurrentIndex);
}

FVector ABlasterCharacter::GetLocationFromCamera(FVector Target) const
{
	//UE_LOG(LogTemp, Error, TEXT("Distance Aim: [CameraBoom] (X=%f,Y=%f,Z=%f)"),
	//	CameraBoom->GetComponentLocation().X,
	//	CameraBoom->GetComponentLocation().Y,
	//	CameraBoom->GetComponentLocation().Z);
	float DistanceFromCamera = UMathCalculation::GetDistance(CameraBoom->GetComponentLocation(), Target);
	return
		FVector(
			CameraBoom->GetComponentLocation().X,
			CameraBoom->GetComponentLocation().Y,
			CameraBoom->GetComponentLocation().Z);
}

FTransform ABlasterCharacter::GetADSRightHandTransform() const
{
	return ADSRightHandTransform;
}

FGeneratePath ABlasterCharacter::GetPathStats() const
{
	return PathStats;
}

FVector ABlasterCharacter::GetOptimizePath(bool IsInRangePathStats)
{
	FVector Location;
	FVector CharacterLocationInIdlePath = ConvertLocationIntoIdlePath(GetActorLocation());

	if (IdlePath)
	{
		if (IsInRangePathStats)
		{
			UE_LOG(LogTemp, Error, TEXT("In range Path"));

			int32 ShortestIndex = IdlePath->ShortestDistancePathIndex(CharacterLocationInIdlePath, PathStats);
			SetShortestDistanceIndex(ShortestIndex);
			FVector ShortestDistancePathPoint = IdlePath->GetIdlePoint(PathStats.ShortestDistanceIndex);

			Location = IdlePath->GeneratePathPoint(CharacterLocationInIdlePath, ShortestDistancePathPoint, PathStats);

			//debug
			FVector Target = IdlePath->GetIdlePoint(PathStats.ShortestDistanceIndex);
			Target = RollBackCharacterLocation(Target);
			//DrawDebugLine(GetWorld(), GetActorLocation(), Target, FColor::Red, true);


		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Out range Path"));

			Location = IdlePath->ShortestDistanceByVertices(CharacterLocationInIdlePath, PathStats);

			int32 ShortestNextIndex = IdlePath->ShortestDistancePathIndex(Location, PathStats);
			//DrawDebugLine(GetWorld(), RollBackCharacterLocation(Location), RollBackCharacterLocation(IdlePath->GetIdlePoint(ShortestNextIndex)), FColor::Black, true);
			SetShortestDistanceIndex(ShortestNextIndex);
		}

	}
	Location = RollBackCharacterLocation(Location);

	UE_LOG(LogTemp, Error, TEXT("New Loc = %f | %f | %f"), Location.X, Location.Y, Location.Z);
	//DrawDebugLine(GetWorld(), GetActorLocation(), Location, FColor::Blue, true);

	return Location;
}

int32 ABlasterCharacter::GetNextIdlePathIndex() const
{
	FVector LocationInIdlePath = ConvertLocationIntoIdlePath(GetActorLocation());
	if (IdlePath == nullptr) return 0;
	return IdlePath->NextIdlePathIndex(LocationInIdlePath, PathStats);
}

UActorReceiver* ABlasterCharacter::GetActorReceiver() const
{
	return ActorReceiver;
}

URapidFollowPath* ABlasterCharacter::GetRapidFollowPath() const
{
	return RapidFollowPath;
}

TArray<AActor*> ABlasterCharacter::LoadActorReceiver()
{
	if (ActorReceiver == nullptr) return TArray<AActor*>();
	return ActorReceiver->GetObjectsResultData();
}

int32 ABlasterCharacter::GetRangePath()
{
	return 0;
}

ERangePathType ABlasterCharacter::GetRangePathType() const
{
	return PathStats.CurrentLocationRange;
}

FVector ABlasterCharacter::GetIdlePathLocation() const
{
	if (IdlePath == nullptr) return FVector();
	return IdlePath->GetActorLocation();

}

FVector ABlasterCharacter::ConvertLocationIntoIdlePath(FVector GivenLocation) const
{
	FVector IdlePathLocation = GetIdlePathLocation();
	return FVector(
		GivenLocation.X - IdlePathLocation.X,
		GivenLocation.Y - IdlePathLocation.Y,
		GivenLocation.Z - IdlePathLocation.Z);
}

FVector ABlasterCharacter::RollBackCharacterLocation(FVector GivenLocation) const
{
	FVector IdlePathLocation = GetIdlePathLocation();
	return FVector(
		GivenLocation.X + IdlePathLocation.X,
		GivenLocation.Y + IdlePathLocation.Y,
		GivenLocation.Z + IdlePathLocation.Z);
}












