#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"

#include "Blaster/BlasterTypes/Constant.h"
#include "Blaster/Melee/MeleeTypes.h"
#include "Blaster/Weapon/HitboxType.h"
#include "Blaster/Weapon/WeaponComponent.h"
#include "Weapon.generated.h"

class ACasing;
class ABlasterCharacter;
class ABlasterPlayerController;
class UWeaponDetailWidget;
class UTexture2D;
class USoundCue;
class USphereComponent;
class UWidgetComponent;
class UBoxComponent;
class UAnimationAsset;
class UCurveFloat;

USTRUCT(BlueprintType)
struct FDamageHitboxes
{
	GENERATED_BODY();
public:
	UPROPERTY(EditAnywhere)
	float Damage_Head = 0.f;

	UPROPERTY(EditAnywhere)
	float Damage_Neck = 0.f;
	
	UPROPERTY(EditAnywhere)
	float Damage_Body = 0.f;

	UPROPERTY(EditAnywhere)
	float Damage_Hand = 0.f;

	UPROPERTY(EditAnywhere)
	float Damage_Leg = 0.f;
};


UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	
	EWS_MAX UMETA(DisplayName = "DefaultMAX"),

};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetupPickupWidget();
	void SetHUDAmmo();
	void SetHUDBarrel(int32 NumToAdd);
	
	void ShowPickupWidget(bool bShowWidget);
	void LoadWeaponImageToHUD();
	void ReloadWeaponImageToHUD();

	virtual void Fire(const FVector& HitTarget);
	void ReloadWeapon();
	void AmmoSwitchWeapon();
	void SpawnCasing();
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);
	void AddBarrelAmmo(int32 AmmoToAdd);

	void MeleeStart();
	void MeleeEnd();

	void MeleeDetect();
	/**
	* Textures for the weapon crosshairs
	*/
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;

	/**
	* Zoomed POV while aiming
	*/
	UPROPERTY(EditAnywhere)
	float ZoomFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	/**
	* Equip Sound
	*/
	UPROPERTY(EditAnywhere)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere)
	USoundCue* PickupSound;

	/**
	* Empty Sound
	*/
	UPROPERTY(EditAnywhere)
	USoundCue* EmptySound;

	/**
	* Automatic Fire
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic = true;

	/*
	* LeftHandRotation
	*/

	UPROPERTY(EditAnywhere, Category = "LeftHandRotation")
	float LeftHandRotationRoll;

	UPROPERTY(EditAnywhere, Category = "LeftHandRotation")
	float LeftHandRotationYaw;

	UPROPERTY(EditAnywhere, Category = "LeftHandRotation")
	float LeftHandRotationPitch;

	void SetHitboxType(EHitboxType CurrentHitboxType);

	void GenerateRecoilVector();

	void AttachWeaponComponent(EWeaponComponent WeaponComponent);
	void DetachWeaponComponent(EWeaponComponent WeaponComponent);
	
	/*
	* Using with Reload state
	*/
	void ApplyAmmoRefillHand();

	/*
	* Using with BeginPlay and after Reload state
	*/
	void ResetAmmoRefillHand();

protected:
	virtual void BeginPlay() override;

	/**
	* Melee Damage
	*/
	void AddVictimCharacterDamage(FHitResult* VictimCharacterResult);
	//Melee Weapon
	void AddAttackVictimCharacter(FHitResult* VictimCharacterResult);
	//Melee In Range Weapon
	void AddMeleeVictimCharacter(FHitResult* VictimCharacterResult);
	void ResetVictimCharacter();


	/**
	* Pick up Overlap
	*/
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	/**
	* This function is outdate, using it can be no effect in game
	*/
	UFUNCTION()
	virtual void OnAttackOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnAttackOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	/**
	* Hitbox Damage
	*/
	UPROPERTY(EditAnywhere, Category = "Hitbox")
	FDamageHitboxes DamageHitboxes;

	float GetHitbox(ABlasterCharacter* HitCharacter, FHitResult HitBone);

	/**
	* Range
	*/
	float WeaponRange = 0;

	/**
	* Enable / Disable Custom Depth
	*/
	void EnableCustomDepth(bool bEnable);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")	 
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UWidgetComponent* PickupWidget;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponMeleeCollisionBox;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UAnimationAsset* AmmoSwitchAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UAnimationAsset* ReloadAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float AngleAimingYaw = DEFAULT_ANGLE_AIMING_YAW;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bHeavyShoulderWeapon = false; //using in shoulder animation
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bHeavyCarriedWeapon = false; //using normal animation

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bOneHandWeapon = false; //some pistol can be using one hand

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bLightWeapon = false; //pistol, some melee weapons

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float AngleAimingPitch = DEFAULT_ANGLE_AIMING_PITCH;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bUsingMagazine = true;
	
	/*
	* some weapons will require using this as damage 
	* (Except Grenade and Rocket as known can radius damage, ignore this setting)
	* EX: Pistol shoot with projectile hook grenade)
	*/
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bProjectileDamageRadius = false;  


	/*
	* Default is 0, change ammo to refill in hand to check
	*/
	int32 AmmoRefillHand = 0; //default 
	
	/*
	* Default is 0, ammo will check to make 
	* sure hand is fill until empty 
	* begin with CurrentAmmoRefillHand = AmmoRefillHand 
	* end with CurrentAmmoRefillHand == 0
	*/
	int32 CurrentAmmoRefillHand = 0;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACasing> CasingClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY()
	ABlasterCharacter* OwnerCharacter;

	UPROPERTY()
	ABlasterPlayerController* OwnerController;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	/*
	* HUD: Image
	*/
	bool bLoadedImage = false;

	FString WeaponName = "";
	UTexture2D* WeaponImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* BarrelImage;

	/*
	* this class wont be show in blueprint, but can be check available
	* class that using own widget
	*/
	UWeaponDetailWidget* WeaponDetailWidget;

	/**
	* Number Of Barrels
	*/
	UPROPERTY(EditAnywhere, Category = "Multi Barrel")
	bool bMultiBarrel;

	UPROPERTY(EditAnywhere, Category = "Multi Barrel")
	int32 Barrel = 1;

	UFUNCTION()
	void OnRep_Barrel();

	void SpendBarrelRound();

	UPROPERTY(EditAnywhere, Category = "Multi Barrel")
	int32 BarrelAmmo = 1;

	UPROPERTY(EditAnywhere, Category = "Multi Barrel")
	bool bAllowedAmmoSwitch = false;

	bool bWeaponDrop = false;

	/**
	* Check Foregrip and Grip (Weapon Attachment)
	*/

	UPROPERTY(EditAnywhere, Category = "Foregrip Setting")
	bool bHoldingForegrip = false;

	UPROPERTY(EditAnywhere, Category = "Foregrip Setting")
	bool bStrapEquipped = false;

	UPROPERTY(EditAnywhere, Category = "Stock Setting")
	bool bHoldingStock = false;

	UPROPERTY(EditAnywhere, Category = "Sniper Setting")
	bool bBoltActionEnable = false;
	
	UPROPERTY(EditAnywhere, Category = "Sniper Setting")
	float BoltActionDelay = 10.f;

	UPROPERTY(EditAnywhere, Category = "Attachment Setting")
	bool bScopeEquipped = false;

	UPROPERTY(EditAnywhere, Category = "Attachment Setting")
	bool bMuzzleEquipped = false;

	FVector CharacterPOV;



	/**
	* Recoil
	*/
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilMagnitude;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilMaxLeft;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilMaxRight;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilMinMultiplier;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	UCurveFloat* HorizonalCurve;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	UCurveFloat* VerticalCurve;

	EHitboxType HitboxType = EHitboxType::EHT_MAX;

	/*
	* Melee (All)
	*/
	TMap<FHitResult*, float> VictimCharacterDamage;

	bool bEnableStun = false;



	
public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EHitboxType GetHitboxType() const { return HitboxType; }

	float GetWeaponRange() const;

	bool IsFull();
	bool IsEmpty();	
	bool IsBarrelEmpty();
	bool IsWeaponOwner(ABlasterCharacter* TargetCharacter);
	bool IsMeleeWeapon();
	bool IsEmptyAmmoRefillHand();

	FORCEINLINE bool IsWeaponDrop() const { return bWeaponDrop; };
	FORCEINLINE bool IsMagCapacity() const { return Ammo == MagCapacity; }
	FORCEINLINE bool IsHoldingForegrip() const { return bHoldingForegrip ? bHoldingForegrip : false; }
	FORCEINLINE bool IsHoldingStock() const { return bHoldingStock ? bHoldingStock : false; }
	FORCEINLINE bool IsMultiBarrel() const { return bMultiBarrel; }
	FORCEINLINE bool IsAllowedAmmoSwitch() const { return bAllowedAmmoSwitch; }
	FORCEINLINE bool IsScopeEquipped() const { return bScopeEquipped; }
	FORCEINLINE bool IsHeavyShoulderWeapon() const { return bHeavyShoulderWeapon; }
	FORCEINLINE bool IsHeavyCarriedWeapon() const {
		return bHeavyCarriedWeapon;
	}
	FORCEINLINE bool IsOneHandWeapon() const { return bOneHandWeapon; }
	FORCEINLINE bool IsLightWeapon() const {
		return bLightWeapon;
	}

	FORCEINLINE bool IsUsingMagazine() const { return bUsingMagazine; }
	FORCEINLINE bool IsEnableStun() const { return bEnableStun; }
	FORCEINLINE bool IsProjectileDamageRadius() { return bProjectileDamageRadius; }

	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE int32 GetBarrel() const { return Barrel; }
	FORCEINLINE int32 GetBarrelAmmo() const { return BarrelAmmo; }

	FORCEINLINE float GetZoomedFOV() const { return ZoomFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

	FORCEINLINE float GetDamageHead() const { return DamageHitboxes.Damage_Head; }
	FORCEINLINE float GetDamageNeck() const { return DamageHitboxes.Damage_Neck; }
	FORCEINLINE float GetDamageBody() const { return DamageHitboxes.Damage_Body; }
	FORCEINLINE float GetDamageHand() const { return DamageHitboxes.Damage_Hand; }
	FORCEINLINE float GetDamageLeg() const { return DamageHitboxes.Damage_Leg; }

	FORCEINLINE float GetRecoilMagnitude() const { return RecoilMagnitude; }
	FORCEINLINE float GetRecoilMaxLeft() const { return RecoilMaxLeft; }
	FORCEINLINE float GetRecoilMaxRight() const { return RecoilMaxRight; }
	FORCEINLINE float GetRecoilMinMultiplier() const { return RecoilMinMultiplier; }
	FORCEINLINE float GetAngleAimingPitch() const { return AngleAimingPitch; }
	FORCEINLINE float GetAngleAimingYaw() const { return AngleAimingYaw; }
	
	FORCEINLINE UCurveFloat* GetHorizonalCurve() const { return HorizonalCurve; }
	FORCEINLINE UCurveFloat* GetVerticalCurve() const { return VerticalCurve; }

	FORCEINLINE UTexture2D* GetBarrelImage() const { return BarrelImage; }

	
	FTransform GetWeaponAimSightTransform(ERelativeTransformSpace Type);
	FTransform GetWeaponMuzzleTransform(ERelativeTransformSpace Type);
	FVector GetWeaponAimSight();
	FVector GetWeaponRelativeLocation();
	
	

};
			