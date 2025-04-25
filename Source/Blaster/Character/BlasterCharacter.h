#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/Class.h"

#include "Blaster/BlasterTypes/Constant.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Blaster/Path/IdlePath.h"
#include "Blaster/Objects/ActorReceiver.h"
#include "Blaster/Objects/Trigger/Trigger.h"
#include "Blaster/Objects/Notification.h"
#include "Blaster/Math/MathCalculation.h"
#include "Blaster/PathChapter/ClearCondition.h"
#include "Blaster/Pickup/PathChapterItemPickup.h"
#include "Blaster/Unarmed/Unarmed.h"
#include "Blaster/Melee/MeleeTypes.h"
#include "Blaster/Weapon/HitBoxType.h"
#include "BlasterCharacter.generated.h"



class USoundCue;
class AController;
class UAnimMontage;

class UInputComponent;
class UCharacterTrajectoryComponent;
class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
class USpringArmComponent;
class UWidgetComponent;
class USpotLightComponent;

class AWeapon;
class UCombatComponent;
class UBuffComponent;
class UCharacterPathChapterComponent;
class ABlasterPlayerController;
class ABlasterPlayerState;
class URapidFollowPath;
class UAIPerceptionStimuliSourceComponent;

UENUM(BlueprintType)
enum class ECharacterCamaraType : uint8
{
	ECCT_Unequipped UMETA(DisplayName = "Unequipped"),
	ECCT_UnequippedSight UMETA(DisplayName = "Unequipped Sight"),
	ECCT_Hipfire UMETA(DisplayName = "Hipfire"),
	ECCT_CrouchHipfire UMETA(DisplayName = "Crouch Hipfire"),
	ECCT_ADS UMETA(DisplayName = "ADS"),
	ECCT_CrouchADS UMETA(DisplayName = "Crouch ADS"),
	ECCT_Down UMETA(DisplayName = "Down"),

	ECCT_Default UMETA(DisplayName = "Default"),
};

UENUM(BlueprintType)
enum class EHitReact : uint8
{
	EHR_UP UMETA(DisplayName = "uP"),
	EHR_DOWN UMETA(DisplayName = "Down"),
	EHR_LEFT UMETA(DisplayName = "Left"),
	EHR_RIGHT UMETA(DisplayName = "Right"),

};

USTRUCT(BlueprintType)
struct FCharacterCamera
{
	GENERATED_BODY();
public:

		UPROPERTY(VisibleAnywhere)
		FVector CameraBoomLocation;

		UPROPERTY(VisibleAnywhere)
		float TargetAimLength;

		UPROPERTY(VisibleAnywhere)
		FVector CameraBoomSocket;

		UPROPERTY(VisibleAnywhere)
		FRotator TPPCameraRotation = FRotator(-32.f, 0.f, 0.f);

		FCharacterCamera()
		{
			TargetAimLength = 0.f;
			CameraBoomSocket = FVector(0);
			TPPCameraRotation = FRotator(0);
		};

		FCharacterCamera(FVector CameraLocation, float Length, FVector SocketLocation, FRotator Rotation)
		{
			CameraBoomLocation = CameraLocation;
			TargetAimLength = Length;
			CameraBoomSocket = SocketLocation;
			TPPCameraRotation = Rotation;
		};
	
	
	//FCharacterCamera(
	//	FVector Location,
	//	float TargetAim,
	//	FRotator Rotation)
	//{

	//	CameraBoomLocation = Location,
	//		TargetAimLength = TargetAim,
	//		TPPCameraRotation = Rotation;
	//};
		
};

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	/**
	* Initialize, state
	*/
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Falling() override;
	//virtual void Landed() override;

	void SetRotationRate();

	virtual void OnRep_ReplicatedMovement() override;
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Elim();
	//UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	/**
	* Run
	*/
	void CheckRunForwardPressed(); //use with Press Run First
	void CheckRunForwardRelease(); //use with Release Run First
	void SetRunningTimer(); //active time for running to slide
	void StopRunning(); //check if slide is active

	/*
	* AI Setup 
	*/
	void AutoPickupEquippedWeapon();
	void ReloadAssetData();
	void SpawnWeaponAtCharacterLocation();

	/**
	* Montage
	*/
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlaySingleReloadMontage(); //play with load single ammo
	void PlayAmmoSwitchMontage();
	void PlayElimMontage();
	void PlayMeleeMontage();
	void PlayHitReactMontage();
	void PlayHitReactMontage(EHitReact HitReact, EHitboxType HitBoxType);
	void PlayAttackMontage(bool bAiming);
	void PlayStunMontage();
	void DisableGettingHit();
	void DestroyCharacter();

	void StopReloadMontage();
	void StopSingleReloadMontage(); //single Reload does not mean end state, return end Reload instead
	void StopAmmoSwitchMontage();
	void StopAttackMontage();

	/**
	* Melee Weapon
	*/
	//UFUNCTION(NetMulticast, Reliable)
	void Melee();
	void MeleeStart();
	void MeleeEnd();

	void MeleeOnTick();
	void StopAttack();

	/**
	* Range Weapon
	*/
	void AIFire(bool bFireActive);
	void AIReload();

	/**
	* Asset Path
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FString> AssetPath;

	/**
	* AI
	*/
	void SetAICharacterSetup();
	void SetAIMovement(float MoveForwardNum, float MoveRightNum);
	void SetMovementAccelerating(bool bAccelerating);
	void SetSightOfPlayer(bool bSightPlayer);

	/*
	* AI Event
	*/

	UFUNCTION(BlueprintImplementableEvent)
	void EndObjective();
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSuspect();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateObjectiveLocation();

	UFUNCTION(BlueprintImplementableEvent)
	void DisableObjectLocation();

	/**
	* Range Weapon
	*/
	void SetPathStats();
	void SetLastestPathStats(int32 LastestIndex);
	void SetShortestDistanceIndex(int32 ShortestIndex);
	void ChooseBestPathBox();
	void ResetCurrentRangePath();
	void SetupRapidFollowPath();

	
	//AI - Detected (Update in future)
	void SetDetectedState(bool DetectedState);

	/**
	* HUD
	*/
	void ShowHUDEndObjective();
	void UpdateHUDHealth();
	void UpdateHUDWeapon();
	void UpdateHUDWeaponBarrel();
	void UpdateHUDPathChapter();
	void UpdateHUDObjective();
	void UpdateHUDCaptureTimer();
	void UpdateHUDNumRequired();
	void DisableHUDCaptureTimer();

	void ShowDamageScreen(AActor* DamageCauser, float Damage); //disable for update

	/**
	* Path Chapter
	*/

	void TriggerPathChapter(FString CollisionName); //trigger: travel, capture
	void PickUpObjectiveItem(); //collect
	void AddToKillWithObjective(AActor* DamageCharacter); //kill collect
	/**
	* First Person Perspective
	*/
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void SwitchPOV();
	void SwitchFlashlight();
	void FirstPersonPOV(bool bFirstPersonChange);
	void FirstPersonAiming();

	void ADS_Weapon(bool IsAiming);

protected:	
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void PrimaryWeaponButtonPressed();
	void SecondaryWeaponButtonPressed();
	void MeleeWeaponButtonPressed();
	void PickupButtonPressed();
	void EquipButtonPressed(); //unavailable
	void DropButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void RunButtonPressed();
	void RunButtonReleased();
	void MeleeButtonPressed();
	void POVButtonPressed();
	void UnequippedButtonPressed();
	void FlashlightButtonPressed();
	void DebugButtonPressed();

	void AimOffset(float DeltaTime);

	void CalculateAO_Pitch();
	void SimProxiesTurn();

	//Jump
	virtual void Jump() override;
	void FallingTimerStart();
	void SetFallingTime();

	//Melee-Range Attack
	void FireButtonPressed();
	void FireButtonReleased();

	void UnarmedMelee();

	/**
	* Slide
	*/
	void Slide();
	void StopSlide();
	void StopSlideStateInterrupt();
	void SetupActorReceiver();
	
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser); 

	void RestoreHealth(); //using only if player go down, health will restore into 100; however, health item later
	void DownHealthInit(); //health will go downward if player state is down

	//Poll for any relelvant classes and initialize our HUD
	void PollInit();

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

	UPROPERTY(BlueprintReadWrite)
	bool bElimmed = false; //elimination
	//USING ONLY WITH BP, after mid term will switch

	/**
	* Player Health
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealthBar();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHealthBar();

private:

	/*
	* Default Sub Object
	*/
	UPROPERTY(VisibleAnywhere, Category = Camera)
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		UCameraComponent* TPPCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		UCameraComponent* FPPCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		USpringArmComponent* CameraADSBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		UCameraComponent* ADSCamera;


	UPROPERTY(VisibleAnywhere, Category = Light)
		USpotLightComponent* FlashlightFPP;

		UPROPERTY(VisibleAnywhere, Category = Light)
		USpotLightComponent* FlashlightTPP;




	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		AWeapon* OverlappingWeapon;

	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCombatComponent* Combat;
		
	UPROPERTY(VisibleAnywhere)
		UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCharacterPathChapterComponent* CharacterPathChapter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* LeftMeleeCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* RightMeleeCollisionBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCharacterTrajectoryComponent* CharacterTrajector;

	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
		void ServerPickupButtonPressed();

	UFUNCTION(Server, Reliable)
		void ServerEquipButtonPressed();

		float AO_Yaw;
		float InterAO_Yaw;
		float AO_Pitch;
		FRotator StartingAimRotation;

		ETurningInPlace TurningInPlace;
		void TurnInPlace(float DeltaTime);

		float YawInput;
		float PitchInput;

		//testing POV 
		UPROPERTY(EditAnywhere, Category = "Test")
		float XAxis = 0.f;

		UPROPERTY(EditAnywhere, Category = "Test")
		float YAxis = 0.f;

		UPROPERTY(EditAnywhere, Category = "Test")
		float ZAxis = 0.f;

		//Jump
		bool bIsInAir;
		bool bFalling = false;

		FTimerHandle JumpFallingTimer;
		float FallingTime = -1.f;

		/**
		* Animation Montages
		*/

		UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* FireWeaponMontage;

		UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* ReloadMontage;

		UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* AmmoSwitchMontage;

		UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* HitReactMontage;
		
		UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* ElimMontage;

		UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* MeleeMontage;

		UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* AttackMontage;
		
		UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* StunMontage;


		/**
		* Melee
		*/
		UPROPERTY(EditAnywhere, Category = Combat)
		float MeleeDamage = 10.f;

		bool bMeleeFinished = false; //count start to end
		bool bMeleeHit = false; //when hit, return the damage and no overlap after take dmg
		bool bMeleeTime = false;

		/*
		* Hit 
		*/
		bool bGettingHit = false;
		FVector TargetGettingHit = FVector();



		//UAnimMontage* Montage;

		void HideCameraIfCharacterClose();

		UPROPERTY(EditAnywhere)
		float CameraThreshold = 80.f;

		bool bRotateRootBone;
		float TurnThreshold = 0.5f;
		FRotator ProxyRotationLastFrame;
		FRotator ProxyRotation;
		float ProxyYaw;
		float TimeSinceLastMovementReplication;
		float CalculateSpeed();



		UFUNCTION()
		void OnRep_Health(float LastHealth);
		
		UPROPERTY(VisibleAnywhere)
		ABlasterPlayerController* BlasterPlayerController;



		/**
		* Player Down and Death
		*/
		bool bDeath = false; //death

		UPROPERTY()
		FTimerHandle DownHealthTimer;

		UPROPERTY(EditDefaultsOnly)
		float DownHealthDelay = 0.025f;

		void DownHealthFinished();
		//will see warmup to set up time with player down time.

		ABlasterPlayerState* BlasterPlayerState;

		/**
		*AI Character 
		*/
		UPROPERTY(EditAnywhere)
		bool bAICharacter = false;

		UPROPERTY(EditAnywhere, Category = "AI")
		bool AllowRapidFollowPath = false;

		UPROPERTY(VisibleAnywhere)
		URapidFollowPath* RapidFollowPath;

		bool bDetectedCharacter;

		bool bCharacterdDetected;

		//UPROPERTY(EditAnywhere, Category = "AI")
		//TSoftClassPtr<URapidFollowPath>RapidFollowPathList;


		/**
		* Spawm Weapon for AI Character (certain gamemode)
		*/
		TArray<FAssetData> AssetData;

		int32 NumAssetData;

		/**
		* Movement Focus AI Character
		*/
		UPROPERTY(VisibleAnywhere)
		bool bMovementAccelerating = false;

		UPROPERTY(VisibleAnywhere)
		bool bSightOfPlayer = false;

		/**
		* AI: Idle Path
		*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
		AIdlePath* IdlePath;

		FGeneratePath PathStats;

		bool IsSetPathStats = false;

		/**
		* Box Debug / Box Range
		*/
		FVector Box;
		FVector BoxExtend;

		/**
		* In Range Path Check
		*/
		ERangePathType CurrentRangePath;

		/**
		* Check Available Object Surround Character (Playstyle + Rapid Follow Path)
		*/
		UPROPERTY(VisibleAnywhere)
		UActorReceiver* ActorReceiver;

		/**
		* Calculate Path
		*/
		UPROPERTY(VisibleAnywhere)
		UCalculatePath* CalculatePath;
		
		// ECharacterCamaraType CharacterCamaraType; disable during no effect


		/**
		* Run
		*/
		bool bForwardPressed = false;
		bool bRunPressed = false;
		bool bCheckRunForwardPressed = false;
		bool bRunState;
		FTimerHandle RunHandle;

		/**
		* Path Chapter
		*/


		//POV define

		const FCharacterCamera CharacterFirstPerson = FCharacterCamera(
			FVector(0.f, 25.f, 170.f), DEFAULT_FIRST_PERSON_CHARACTER_POV, FVector(0.f, 0.f, 0.f), FRotator(0)
		);
		const FCharacterCamera CharacterThirdPerson = FCharacterCamera(
			FVector(-4.f,0.f, 172.f), DEFAULT_THIRD_PERSON_CHARACTER_POV, FVector(0.f, 100.f, 0.f), FRotator(-32.f, 0.f, 0.f)
		);

		const FCharacterCamera CharacterFirstPersonCrouch = FCharacterCamera(
			FVector(-14.f, 50.f, 122.f), DEFAULT_FIRST_PERSON_CHARACTER_POV, FVector(0.f, 0.f, -40.f), FRotator(0)
		);
		//(X=-14.000000,Y=30.000000,Z=122.000000)
		const FCharacterCamera CharacterThirdPersonCrouch = FCharacterCamera(
			FVector(-4.f, 0.f, 172.f), DEFAULT_THIRD_PERSON_CHARACTER_POV, FVector(-10.f, 60.f, -40.f), FRotator(-32.f, 0.f, 0.f)
		);

		/**
		* Aiming
		*/
		UPROPERTY(VisibleAnywhere)
		bool bCharacterSight;

		double DistanceAim = 0.f;
		bool bInFirstPerson = false;
		bool bAimingState = false;

		FTransform ADSRightHandTransform;

		/**
		* Slide
		*/
		UPROPERTY(EditAnywhere, Category = "Movement Setting")
		float SlideSpeed = 2.f;

		bool bEnableSlide = false;
		bool bSlideState = false;
		FTimerHandle SlideHandle;
		FTimerHandle SlideInterruptHandle;


		//other
		UMathCalculation* MathCalculation;
		UPROPERTY(EditAnywhere, Category="Debug Option")
		bool bEnableInvisible = false;
		APawn* PlayerPawn; //this only use in Single player, Multiplayer will update

		/**
		* Check Valid In Sight Target
		*/
		FHitResult FirstTargetSight;

public: 
	void SetOverlappingWeapon(AWeapon* Weapon);
	void SetCharacterCamera(float DeltaTime);
	void SetCharacterCameraFirstPerson(float DeltaTime);
	void SetCharacterRunState(bool IsRunState);

	bool IsWeaponEquipped();
	bool IsAiming();
	bool IsMontageFinished(UAnimMontage* Montage);
	bool IsMontageRunning();
	bool IsRestrictWeapon();
	bool IsInRangePath();
	bool CanEnableRapidFollowPath();
	bool IsWeaponDrop();
	bool IsFPPAiming();
	bool IsInCollectObjective();
	bool IsDamageYourself(AActor* DamageCauser);
	bool IsWeaponCanDamageYourself(AActor* DamageCauser);
	bool IsSameTeam(AActor* TargetActor);


	UFUNCTION(BlueprintCallable)
	bool IsValidTargetCharacterInSight(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	bool IsNearPlayer(); //only in singleplayer with pawn 0

	EHitReact GetHitReactYaw(float HitReactYaw);

	FORCEINLINE float GetYawInput() const { return YawInput; }
	FORCEINLINE float GetPitchInput() const { return PitchInput; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetFallingTime() const { return FallingTime; }

	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetTPPCamera() const { return TPPCamera; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }

	FORCEINLINE bool IsMovingForward() const { return bForwardPressed; }
	FORCEINLINE bool IsRunPressed() const { return bRunPressed; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE bool IsDeath() const { return bDeath; }
	FORCEINLINE bool IsAICharacter() const { return bAICharacter; }
	FORCEINLINE bool IsMovementAccelerating() const { return bMovementAccelerating; }
	FORCEINLINE bool IsSightOfPlayer() const { return bSightOfPlayer; }
	FORCEINLINE bool IsMeleeFinished() const { return bMeleeFinished; }
	FORCEINLINE bool IsDetectedCharacter() const { return bDetectedCharacter; }
	FORCEINLINE bool IsRunState() const { return bRunState; }
	FORCEINLINE bool IsSlideState() const { return bSlideState; }
	FORCEINLINE bool IsGettingHit() const { return bGettingHit; }
	FORCEINLINE bool IsInFirstPerson() const { return bInFirstPerson; }
	FORCEINLINE bool IsAimingState() const { return bAimingState; }
	//FORCEINLINE bool IsInGround() const { return GetWorldTimerManager().GetTimerRemaining(JumpFallingTimer) == -1.f; }

	FORCEINLINE FVector GetTargetGettingHit() const { return TargetGettingHit; }
	FORCEINLINE UCharacterTrajectoryComponent* GetCharacterTrajector() const { return CharacterTrajector; }
	

	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE void SetMeleeFinished(bool isMeleeFinished) { bMeleeFinished = isMeleeFinished; }


	UPROPERTY(EditAnywhere, Category = "WeaponRotationCorrection")
	float RightHandRotationRoll;

	UPROPERTY(EditAnywhere, Category = "WeaponRotationCorrection")
	float RightHandRotationYaw;

	UPROPERTY(EditAnywhere, Category = "WeaponRotationCorrection")
	float RightHandRotationPitch;

	FTransform RightHandTransformLocation;

	UPROPERTY(EditAnywhere)
	USoundCue* RunSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ObjectiveLocation;


	AWeapon* GetEquippedWeapon();
	UCombatComponent* GetCombat();
	UActorReceiver* GetActorReceiver() const;
	URapidFollowPath* GetRapidFollowPath() const;
	AIdlePath* GetIdlePath() const;
	FTransform GetADSRightHandTransform() const;
	FGeneratePath GetPathStats() const;

	FVector GetHitTarget() const;
	FVector GetLatestPathStats() const;
	FVector GetLocationFromCamera(FVector Target) const;
	FVector GetOptimizePath(bool IsInRangePathStats); //migrate to StepBack
	FVector GetIdlePathLocation() const;

	ECombatState GetCombatState() const;
	ERangePathType GetRangePathType() const;

	int32* GetIndexCharacter() const;
	int32 GetRangePath(); //nothing here
	int32 GetNextIdlePathIndex() const;

	TArray<AActor*>LoadActorReceiver();

	FVector ConvertLocationIntoIdlePath(FVector GivenLocation) const;
	FVector RollBackCharacterLocation(FVector GivenLocation) const;

	//int32 BestPathStats(FVector GivenLocation);
};
