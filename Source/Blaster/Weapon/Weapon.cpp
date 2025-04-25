#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "PhysicsEngine/ShapeElem.h"
#include "PhysicsEngine/BodySetup.h"

#include "Kismet/GameplayStatics.h"  //consider if change overlap fucntion

#include "Blaster/BlasterTypes/Constant.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/Weapon/HitboxType.h"
#include "Blaster/Weapon/WeaponComponent.h"
#include "Blaster/Spawn/SpawnController.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/HUD/WeaponDetailWidget.h"

#include "ProjectileWeapon.h"
#include "HitScanWeapon.h"

AWeapon::AWeapon()
{

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
	PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PickupWidget->SetDrawAtDesiredSize(true);

	//TODO: Create New Own User-Widget to access the weapon name and database
	//PickupWidget->SetWidgetClass()

	WeaponMeleeCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponMeleeCollisionBox"));
	WeaponMeleeCollisionBox->SetupAttachment(RootComponent);


	//if (WeaponMeleeCollisionBox)
	//{
	//	FAttachmentTransformRules const Rules{
	//		EAttachmentRule::SnapToTarget,
	//		EAttachmentRule::SnapToTarget,
	//		EAttachmentRule::KeepWorld,
	//		false
	//	};
	//	FName MeleeSocket = TEXT("Melee_Damage_Socket");
	//	//FVector DefaultSocketLocation{0.f, 0.f, 0.f};

	//	WeaponMeleeCollisionBox->AttachToComponent(WeaponMesh, Rules, MeleeSocket);
	//	//WeaponMeleeCollisionBox->SetRelativeLocation(DefaultSocketLocation);
	//}

}

void AWeapon::SetHitboxType(EHitboxType CurrentHitboxType)
{
	HitboxType = CurrentHitboxType;
}

void AWeapon::GenerateRecoilVector()
{
	float Angle = FMath::RandRange(RecoilMaxLeft, -RecoilMaxRight);
	float recMag = RecoilMagnitude;
	float tempMag = -FMath::RandRange(recMag * RecoilMinMultiplier, recMag);

	float RecoilYaw = FMath::Sin(FMath::DegreesToRadians(Angle));
	float RecoilPitch = FMath::Sin(FMath::DegreesToRadians(Angle));

	float ResultRecoilYaw = tempMag;
	float ResultRecoilPitch = -tempMag;
}

void AWeapon::AttachWeaponComponent(EWeaponComponent WeaponComponent)
{
	FName SocketName = "";
	switch (WeaponComponent)
	{
	case EWeaponComponent::EWC_Magazine:
		SocketName = "b_gun_mag";
		break;
	}
	WeaponMesh->UnHideBoneByName(SocketName);
}

void AWeapon::DetachWeaponComponent(EWeaponComponent WeaponComponent)
{
	FName SocketName = "";
	switch (WeaponComponent)
	{
	case EWeaponComponent::EWC_Magazine:
		SocketName = "b_gun_mag";
		break;
	}
	WeaponMesh->HideBoneByName(SocketName, EPhysBodyOp::PBO_MAX);
}

void AWeapon::ApplyAmmoRefillHand()
{
	if (IsEmptyAmmoRefillHand())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString("End Filled"));
		CurrentAmmoRefillHand = AmmoRefillHand - 1;
	}
	else --CurrentAmmoRefillHand;
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("Ammo Refill = ") + FString::FromInt(CurrentAmmoRefillHand));
}

void AWeapon::ResetAmmoRefillHand()
{
	CurrentAmmoRefillHand = AmmoRefillHand;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	if (HasAuthority())
	{
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	}

	SetupPickupWidget();

	SetWeaponState(EWeaponState::EWS_Dropped);
	//WeaponMeleeCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnAttackOverlapBegin);
	//WeaponMeleeCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnAttackOverlapEnd);

	ResetAmmoRefillHand();
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MeleeDetect();
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, Ammo);
}

void AWeapon::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

	ABlasterCharacter* CurrentCharacter = Cast<ABlasterCharacter>(OtherActor);

	if (CurrentCharacter == nullptr) return;

	if (CurrentCharacter->IsElimmed() && bWeaponDrop)
	{
		bWeaponDrop = true;
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("In the weapon with %d %d"),
		CurrentCharacter->IsElimmed() ? 1 : 0,
		CurrentCharacter->IsDeath() ? 1 : 0);
	if (CurrentCharacter && !CurrentCharacter->IsElimmed() && !CurrentCharacter->IsDeath())
	{
		CurrentCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* CurrentCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (CurrentCharacter)
	{
		CurrentCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::OnAttackOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	//Unarmed->Attack();
	APawn* OwnerPawn = Cast<APawn>(this->GetOwner());
	if (OwnerPawn == nullptr ||
		OtherActor == this->GetOwner() ||
		OtherActor->GetName().Contains(OwnerPawn->GetName())) return;

	UE_LOG(LogTemp, Warning, TEXT("Doing damage by %s"), *OtherActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Doing damage by %s"), *OwnerPawn->GetName());

	AController* InstigatorController = OwnerPawn->GetController();

	if (InstigatorController && HasAuthority())
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			MELEE_DAMAGE,
			InstigatorController,
			this,
			UDamageType::StaticClass());
	}

}

void AWeapon::OnAttackOverlapEnd(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
}

void AWeapon::MeleeDetect()
{
	if (this == nullptr || this->GetOwner() == nullptr) return;
	FCollisionShape Box = FCollisionShape::MakeBox(WeaponMeleeCollisionBox->GetScaledBoxExtent());

	TArray<FHitResult> CurrentDamageCharacter;

	bool IsCollide = GetWorld()->SweepMultiByChannel(
		CurrentDamageCharacter,
		GetWeaponMesh()->GetSocketLocation("CollisionStartSocket"),
		GetWeaponMesh()->GetSocketLocation("CollisionEndSocket"),
		WeaponMeleeCollisionBox->GetComponentQuat(), //FQuat::Identity, 
		ECollisionChannel::ECC_Pawn,
		Box);
	//this can still interact with melee weapon and range melee weapon
	//we will fix to make more logical in the future

	if (IsCollide)
	{
		//TArray<FHitResult> CurrentDamageCharacter;
		FString Value = "";
		for (FHitResult Result : CurrentDamageCharacter)
		{
			//if (VictimCharacterDamage.Contains(Result)) return;
			FString Name = Result.GetActor()->GetName();
			if (Name.Contains(this->GetOwner()->GetName()) || Name.Contains(this->GetName())) continue;

			Value += Result.GetActor()->GetName() + " ";

			AddVictimCharacterDamage(&Result);
		}
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString(Value));
	}
	//debug
	//DrawDebugBox(GetWorld(), GetWeaponMesh()->GetComponentLocation(), Box.GetExtent(), FColor::Red, true, 3.f);
	//DrawDebugLine(
	//	GetWorld(),
	//	GetWeaponMesh()->GetSocketLocation("CollisionStartSocket"),
	//	GetWeaponMesh()->GetSocketLocation("CollisionEndSocket"),
	//	FColor::Red,
	//	true,
	//	1.f);
}

void AWeapon::AddVictimCharacterDamage(FHitResult* VictimCharacterResult)
{
	IsMeleeWeapon() ?
		AddAttackVictimCharacter(VictimCharacterResult) :
		AddMeleeVictimCharacter(VictimCharacterResult);
}
void AWeapon::AddAttackVictimCharacter(FHitResult* VictimCharacterResult)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	AController* InstigatorController = OwnerPawn->GetController();
	ABlasterCharacter* VictimCharacter = Cast<ABlasterCharacter>(VictimCharacterResult->GetActor());

	if (!HasAuthority() ||
		InstigatorController == nullptr ||
		VictimCharacter == nullptr) return;

	//if (VictimCharacterDamage.Contains(VictimCharacterResult)) return;

	FHitResult Hitbox = *VictimCharacterResult;
	float DamageToCause = GetHitbox(VictimCharacter, Hitbox);
	VictimCharacterDamage.Emplace(VictimCharacterResult, DamageToCause);

	UGameplayStatics::ApplyDamage(
		VictimCharacter,
		DamageToCause,
		InstigatorController,
		this,
		UDamageType::StaticClass()
	);
}

void AWeapon::AddMeleeVictimCharacter(FHitResult* VictimCharacterResult)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	AController* InstigatorController = OwnerPawn->GetController();
	ABlasterCharacter* VictimCharacter = Cast<ABlasterCharacter>(VictimCharacterResult->GetActor());

	if (!HasAuthority() ||
		InstigatorController == nullptr ||
		VictimCharacter == nullptr) return;

	//if (VictimCharacterDamage.Contains(VictimCharacterResult)) return;

	if (VictimCharacterDamage.Contains(VictimCharacterResult)) return;

	VictimCharacterDamage.Emplace(VictimCharacterResult, MELEE_DAMAGE);

	UGameplayStatics::ApplyDamage(
		VictimCharacter,
		MELEE_DAMAGE,
		InstigatorController,
		this,
		UDamageType::StaticClass()
	);
}

void AWeapon::ResetVictimCharacter()
{
	VictimCharacterDamage.Reset();
}

float AWeapon::GetHitbox(class ABlasterCharacter* HitCharacter, FHitResult HitBone)
{
	float DamageToCause = 0.f;
	FBodyInstance* CharacterBoneInstance = HitCharacter->GetMesh()->GetBodyInstance(HitBone.BoneName);
	if (CharacterBoneInstance)
	{
		UBodySetup* CharacterBoneBody = CharacterBoneInstance->GetBodySetup();
		if (CharacterBoneBody)
		{
			FKShapeElem* CharacterBoneHitbox = CharacterBoneBody->AggGeom.GetElement(0);
			//CharacterBoneBody->AggGeom.GetElement(0);
			if (CharacterBoneHitbox)
			{
				//TODO: Hash later
				FString Hitbox = CharacterBoneHitbox->GetName().ToString();

				if (Hitbox.Contains("leg"))
				{
					DamageToCause = DamageHitboxes.Damage_Leg;
					HitboxType = EHitboxType::EHT_Leg;
				}
				if (Hitbox.Contains("hand_l"))
				{
					DamageToCause = DamageHitboxes.Damage_Hand;
					HitboxType = EHitboxType::EHT_HandLeft;
				}
				if (Hitbox.Contains("hand_r"))
				{
					DamageToCause = DamageHitboxes.Damage_Hand;
					HitboxType = EHitboxType::EHT_HandRight;
				}
				if (Hitbox.Contains("body"))
				{
					DamageToCause = DamageHitboxes.Damage_Body;
					HitboxType = EHitboxType::EHT_Body;
				}
				if (Hitbox.Contains("neck"))
				{
					DamageToCause = DamageHitboxes.Damage_Neck;
					HitboxType = EHitboxType::EHT_Neck;
				}
				if (Hitbox.Contains("headshot"))
				{
					DamageToCause = DamageHitboxes.Damage_Head;
					HitboxType = EHitboxType::EHT_Head;
				}
				UE_LOG(LogTemp, Error, TEXT("Gun is Shot in %s with %f dmg"), *CharacterBoneHitbox->GetName().ToString(), DamageToCause);

			}
		}
	}
	return DamageToCause;
}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeapon::SetHUDAmmo()
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : OwnerCharacter;
	if (OwnerCharacter)
	{
		OwnerController = OwnerController == nullptr ?
			Cast <ABlasterPlayerController>(OwnerCharacter->Controller) : OwnerController;
		if (OwnerController)
		{
			OwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::SetHUDBarrel(int32 NumToAdd)
{
	OwnerCharacter = OwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : OwnerCharacter;
	if (OwnerCharacter)
	{
		OwnerController = OwnerController == nullptr ?
			Cast <ABlasterPlayerController>(OwnerCharacter->Controller) : OwnerController;
		if (OwnerController)
		{
			OwnerController->SetHUDBarrelRemaining(NumToAdd);
		}
	}
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	//UE_LOG(LogTemp, Warning, TEXT("Have Multi Barrel = %s"), bMultiBarrel ? TEXT("True") : TEXT("False"));
	if (bMultiBarrel)
	{
		SpendBarrelRound();
	}

	SetHUDAmmo();
}

void AWeapon::SpendBarrelRound()
{
	BarrelAmmo = FMath::Clamp(BarrelAmmo - 1, 0, Barrel);
	UE_LOG(LogTemp, Warning, TEXT("Remain barrel ammo = %d"), BarrelAmmo);
	SetHUDBarrel(-1);
}

void AWeapon::OnRep_Ammo()
{
	//Set State to End Reload State right here. However this code below will outdate
	// as remind for code already using for jumping end state when player (AI, online
	//player for instance)
	OwnerCharacter =
		OwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : OwnerCharacter;
	if (OwnerCharacter && OwnerCharacter->GetCombat() && IsFull())
	{
		//Jump to reload state / shotgun
		//Character->StopSingleReloadMontage();
	}
	SetHUDAmmo();
}

void AWeapon::OnRep_Barrel()
{
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		OwnerCharacter = nullptr;
		OwnerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}

}

void AWeapon::SetupPickupWidget()
{
	if (PickupWidget == nullptr) return;

	WeaponDetailWidget = Cast<UWeaponDetailWidget>(PickupWidget->GetWidget());
	if (WeaponDetailWidget)
	{
		//Weapon Name
		if (WeaponName == "")
		{
			WeaponName = GetName();
			WeaponName.RemoveAt(0, 3);
			WeaponName.RemoveAt(WeaponName.Find(FString("_"), ESearchCase::IgnoreCase), WeaponName.Len() - 1);
		}

		//Weapon Type
		FString WeaponTypeFull = UEnum::GetDisplayValueAsText(WeaponType).ToString();
		FString WeaponTypeAcronyms = "";
		for (FString::ElementType Character : WeaponTypeFull)
		{
			if (FChar::IsUpper(Character)) WeaponTypeAcronyms += Character;

		}
		
		//we will test for damage and range as saving for valid value
		AProjectileWeapon* ProjectileWeapon = Cast<AProjectileWeapon>(this);

		//Damage: 
		//- base Body Damage (all), 
		//- (Projectile),
		//- (Base Damage Body* Pellet) / ? (Shogun) consider
		//Range

		float BaseDamage = 0.f;
		if (ProjectileWeapon)
		{
			BaseDamage = ProjectileWeapon->GetBaseDamage();
			BaseDamage = (BaseDamage == -1.f) ? GetDamageBody() : BaseDamage;
		}
		else BaseDamage = GetDamageBody(); //as hit scan and melee weapon cannot have explosive


		//Range
		//- Projectile: Range by getting Velocity then getting falloff
		//- Hit scan: falloff (update later)

		if (WeaponType == EWeaponType::EWT_MeleeWeapon) WeaponRange = 0.08f;
		else if (ProjectileWeapon)
			ProjectileWeapon->SetWeaponRange();
		else WeaponRange = 1.f; 
		//default for hit scan as no infomation right now
	

		//if (AHitScanWeapon* HitScanWeapon = Cast<AHitScanWeapon>(this)) HitScanWeapon->SetWeaponRange();


		WeaponDetailWidget->SetWeaponStat(IsMeleeWeapon(), BaseDamage, GetWeaponRange(), RecoilMagnitude, FireDelay);
		WeaponDetailWidget->SetWeaponNameType(WeaponName, WeaponTypeAcronyms);

		FTimerHandle ReloadHandle;
		GetWorld()->GetTimerManager().SetTimer(ReloadHandle, this, &AWeapon::LoadWeaponImageToHUD, 1.f);
		//TODO: Check other computer condition: low/high spec computer
	}



	PickupWidget->SetVisibility(false);

}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (bStrapEquipped)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}

		EnableCustomDepth(false);

		break;


	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
		WeaponMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);

		break;
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (bStrapEquipped)
		{
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponMesh->SetEnableGravity(true);
			WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}

		EnableCustomDepth(false);

		break;
	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		
		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
		WeaponMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);

		break;
	}
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{ //this is the line code will change for showing weapon detail widget
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);

	}
}

void AWeapon::LoadWeaponImageToHUD()
{
	ABlasterGameMode* GameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if (GameMode && GameMode->HasMatchStarted() && GameMode->ImageData.IsEmpty() == false)
	{
		bLoadedImage = true;
		for (UTexture2D* Image : GameMode->ImageData)
		{
			if (Image->GetName().Contains(WeaponName))
			{
				WeaponImage = Image;
				break;
			}
		}

	}
	if (bLoadedImage) ReloadWeaponImageToHUD();
}

void AWeapon::ReloadWeaponImageToHUD()
{
	if (WeaponImage)
	{
		WeaponDetailWidget->SetWeaponImage(WeaponImage);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation) {
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	SpawnCasing();
	SpendRound();
	//DrawDebugSphere(GetWorld(), HitTarget,12.f, 12, FColor::Cyan, true);
	//TODO: Test with where ammo goes
}


void AWeapon::AmmoSwitchWeapon()
{
	if (AmmoSwitchAnimation) {
		WeaponMesh->PlayAnimation(AmmoSwitchAnimation, false);
	}
}

void AWeapon::ReloadWeapon()
{
	if (ReloadAnimation) {
		WeaponMesh->PlayAnimation(ReloadAnimation, false);
	}
}

void AWeapon::SpawnCasing()
{
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("b_gun_AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);


			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);

			}
		}
	}
}

void AWeapon::MeleeStart()
{
	SetActorTickEnabled(true);
	bEnableStun = true;
}

void AWeapon::MeleeEnd()
{
	SetActorTickEnabled(false);
	bEnableStun = false;
	ResetVictimCharacter();
}

void AWeapon::Dropped()
{
	bWeaponDrop = true;
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	OwnerCharacter = nullptr;
	OwnerController = nullptr;
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}

void AWeapon::AddBarrelAmmo(int32 AmmoToAdd)
{
	BarrelAmmo = AmmoToAdd;
	SetHUDBarrel(AmmoToAdd);
	//UE_LOG(LogTemp, Warning, TEXT("After Barrel = %d"), BarrelAmmo);
}

float AWeapon::GetWeaponRange() const
{
	return WeaponRange;
}

bool AWeapon::IsFull()
{
	return Ammo == MagCapacity;
}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

bool AWeapon::IsBarrelEmpty()
{
	//if(BarrelAmmo <= 0)
		//UE_LOG(LogTemp, Warning, TEXT("Barrel is empty, reloading..."));
	return BarrelAmmo <= 0;
}

bool AWeapon::IsWeaponOwner(ABlasterCharacter* TargetCharacter)
{
	ABlasterCharacter* WeaponOwner = Cast<ABlasterCharacter>(GetOwner());
	return TargetCharacter->GetName().Contains(WeaponOwner->GetName());
}

bool AWeapon::IsMeleeWeapon()
{
	return GetWeaponType() == EWeaponType::EWT_MeleeWeapon;
}

bool AWeapon::IsEmptyAmmoRefillHand()
{
	return CurrentAmmoRefillHand <= 0;
}

FVector AWeapon::GetWeaponAimSight()
{
	//const USkeletalMeshSocket* WeaponSight = WeaponMesh->GetSocketByName(FName("Aim_IronSight_Socket"));
	//if (WeaponSight)
	//{
	//	return WeaponSight->GetSocketLocation();
	//}

	// return FVector();
	if (WeaponMesh)
	{
		FTransform IronSightTransform = WeaponMesh->GetSocketTransform(FName("ADS_Socket"), ERelativeTransformSpace::RTS_World);
		return IronSightTransform.GetLocation();
	}
	return FVector();
}

FTransform AWeapon::GetWeaponAimSightTransform(ERelativeTransformSpace Type)
{
	if (WeaponMesh)
	{
		return WeaponMesh->GetSocketTransform(
			FName("ADS_Socket"), Type);

	}
	return FTransform();
}

FTransform AWeapon::GetWeaponMuzzleTransform(ERelativeTransformSpace Type)
{
	if (WeaponMesh)
	{
		return WeaponMesh->GetSocketTransform(
			FName("Muzzle"), Type);

	}
	return FTransform();
}

FVector AWeapon::GetWeaponRelativeLocation()
{

	if (WeaponMesh)
	{
		FTransform IronSightTransform = WeaponMesh->GetRelativeTransform();
		return IronSightTransform.GetLocation();
	}
	return FVector();
}

