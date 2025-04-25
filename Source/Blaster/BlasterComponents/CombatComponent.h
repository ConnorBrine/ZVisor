#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"

#include "Blaster/BlasterTypes/CharacterMovementState.h"
#include "Blaster/HUD/BlasterHUD.h" 
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/Weapon/WeaponComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/Objects/HandAttachObject.h"

#include "CombatComponent.generated.h"

class AWeapon;
class ABlasterCharacter;
class ABlasterPlayerController;
class ABlasterHUD;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetHitTargetWithCrosshair(float DeltaTime);
	/**
	* TODO: EquipWeapon will using when character using Middle Mouse Button scroll 
	*/

	/**
	* Equipped and Pickup Weapon
	*/
	void PickupWeapon(AWeapon* WeaponToPickup);
	void PickupPrimaryWeapon(AWeapon* WeaponToPickup);
	void PickupSecondaryWeapon(AWeapon* WeaponToPickup);

	void EquipWeapon(); 
	void EquipPrimaryWeapon();
	void EquipSecondaryWeapon();
	//void EquipMeleeWeapon(); //using later
	void UnequippedWeapon();

	void SetupHandAttachObject();
	void AttachWeaponToRightHand(AWeapon* WeaponToEquip);
	void AttachPrimaryWeaponToBody(AWeapon* WeaponToEquip);
	void AttachSecondaryWeaponToBody(AWeapon* WeaponToEquip);

	void DropEquippedWeapon();
	void DropEquippedPrimaryWeapon();
	void DropEquippedSecondaryWeapon();

	/**
	* Sound
	*/
	void PlayRunningSound();
	void PlayPickupWeaponSound();
	void PlayEquipWeaponSound();
	void PlayEmptyWeaponSound();

	//add stop animation when character switch weapon or...

	/**
	*  Reload and Ammo Switch
	*/
	void ReloadEmptyWeapon(AWeapon* WeaponToReload);

	void Reload();
	void AmmoSwitch();

	UFUNCTION(BlueprintCallable)
	void SingleAmmoReload();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	int32 AmountToReload();

	UFUNCTION(BlueprintCallable)
	void CheckAmmoReload();

	UFUNCTION(BlueprintCallable)
	void FinishAmmoSwitch();
	int32 RestoreAmmoSwitch();

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);

	/**
	* Fire - Attack  
	*/	
	void SetFireReloading(bool bActiveFire);

	UFUNCTION(BlueprintCallable)
	void FinishAttack();

	void Melee();

	/**
	* Run
	*/
	void Running();
	void StopRunning();
	void SetCharacterSpeed();

	/**
	* Hit - Death
	*/
	UFUNCTION(BlueprintCallable)
	void FinishHitReact();

	void CheckIsPlayerDown(bool bIsElim);

	void ApplyWeaponEquippedRecoil();
	void AIAimingRange(FVector TargetHit);

	/*
	* Recoil
	*/
	void GenerateRecoilVector();
	void ResetRecoil(float DeltaTime);
	/**
	* Hand Attach
	*/ 
	//Hand Attach is meaning the detail/addition of weapon attachment/items
	void HandAttachMagazine(const USkeletalMeshSocket* HandSocket, FString WeaponName); //weapon have magazine
	void HandAttachAmmo(const USkeletalMeshSocket* HandSocket, FString WeaponName); //sniper (bolt-action), rocket, grenade launcher

	UFUNCTION(BlueprintCallable)
	void HandAttachObject(bool bFirstMesh); //world change when user using anything relate to hand (pickup, reload, interact,...)

	UFUNCTION(BlueprintCallable)
	void HandDetachObject();

	UFUNCTION(BlueprintCallable)
	void DestroyHandAttach();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponComponent(EWeaponComponent WeaponComponent);

	UFUNCTION(BlueprintCallable)
	void DetachWeaponComponent(EWeaponComponent WeaponComponent);

	/**
	* Slide
	*/
	UFUNCTION(BlueprintCallable)
	void FinishSlide();

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	void SetRangeAiming(bool bIsAiming);
	void SetMeleeAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_EquippedPrimaryWeapon();

	UFUNCTION()
	void OnRep_EquippedSecondaryWeapon();


	void FireButtonPressed(bool bPressed);
	void RangeWeaponButtonPressed();
	void MeleeWeaponButtonPressed();

	void Fire();
	void Attack();

	void RunButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable) 
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	
	void TraceUnderADSWeapon(FHitResult& TraceHitResult);

	void TraceAICharacter(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	void ClearHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();

	void HandleAmmoSwitch();

	void SetHitReactState();

	void StopReloadAndAmmoSwitch();

	void SetSlideState();

private:
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	ABlasterPlayerController* Controller;

	UPROPERTY()
	ABlasterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedPrimaryWeapon)
	AWeapon* EquippedPrimaryWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedSecondaryWeapon)
	AWeapon* EquippedSecondaryWeapon;

	ECharacterMovementState CharacterMovementState;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere, Category = "Character Movement Speed")
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere, Category = "Character Movement Speed")
	float AimWalkSpeed;

	UPROPERTY(EditAnywhere, Category = "Character Movement Speed")
	float RunSpeed;

	UPROPERTY(EditAnywhere, Category = "Character Movement Speed")
	float LightWeaponEquippedSpeedReduce; //pistol, melee weapon

	UPROPERTY(EditAnywhere, Category = "Character Movement Speed")
	float WeaponEquippedSpeedReduce; //this restrict with melee weapon and pistol

	UPROPERTY(EditAnywhere, Category = "Character Movement Speed")
	float HeavyWeaponEquippedSpeedReduce; //Rocket, LMG

	UPROPERTY(EditAnywhere, Category = "Character Movement Speed")
	float BaseReduce;

	UPROPERTY(EditAnywhere, Category = "Character Movement Speed")
	float SlideSpeed;

	bool bFireButtonPressed;

	bool bRunButtonPressed;

	bool bSlide = false;

	bool bTimeFinished = true;

	bool bIsDown = false;

	bool bWeaponDrop = false;

	bool bStartRecoil = false;

	/**
	* HUD and crosshairs
	*/
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FVector HitTarget;

	FHUDPackage HUDPackage;

	/**
	* Aiming and POV
	*/

	// Field Of View when not aiming; set to the camera's base POV in BeginPlay
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	/**
	* Only work with TPP
	*/
	void InterpFOV(float DeltaTime);

	/**
	* Automatic Fire
	*/
	FTimerHandle FireTimer;
	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();


	/**
	* Carried Ammo
	*/
	//Carried ammo for the currently-equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo;
	
	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo;
	
	UPROPERTY(EditAnywhere)
	int32 StartingGrenadeLauncherAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingSRAmmo;

	TMap<EWeaponType, int32> CarriedAmmoMap;
	
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 999999; 
	//TODO: this goal game project did not including maximum AMMO, backpack will replace this later

	void InitializeCarriedAmmo();

	/**
	* Combat State
	*/
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	bool IsExceptionHitResult();

	/**
	* Reload - Ammo Switch
	*/
	UFUNCTION(Server, Reliable)
	void ServerAmmoSwitch();

	void UpdateCarriedAmmo();

	void UpdateAmmoValue();
	//UpdateCarriedAmmo is the state without affect reload value / or the shorten state

	void UpdateSingleAmmoValue(); //bolt-action, shotgun single ammo reload

	void RestoreBarrelValue();

	//Vector Storage
	FVector LastTargetLocation;

	/**
	* Recoil
	*/
	FTimeline RecoilTimeLine;
	
	UFUNCTION()
	void StartHorizonalRecoil(float Value);
	
	UFUNCTION()
	void StartVerticalRecoil(float Value);

	void StartRecoil();
	void ReverseRecoil();

	FRotator RecoilStartRotation;

	bool bFirstFire = false;
	bool bResetRecoil = false; //check is have any reset recoil yet

	int32 NoEquippedWeapon = 0;
	int32 NoInputChange = 0;

	float RecoilYaw;
	float RecoilPitch;

	int32 WeaponEquippedNum = 0;

	/**
	* Hand Attach
	*/
	AHandAttachObject* CurrentHandAttachObject;

	TArray<AActor*> AmmoAttach;
	TArray<AActor*> MagAttach;

public:	
	/**
	* This code will apply which some condition:
	* 1. When players reloading weapon, return Magazine
	* 2. Equip Weapon Component, check which component is affect
	*/
	UFUNCTION(BlueprintCallable)
	const EWeaponComponent GetWeaponComponentAffect();
	
	float GetCharacterMovementState();
	float GetCharacterMovementReduce();

	FORCEINLINE bool IsFireButtonPressed() const{ return bFireButtonPressed; }
	bool IsAvailableCarriedAmmoWeapon();
		
};
