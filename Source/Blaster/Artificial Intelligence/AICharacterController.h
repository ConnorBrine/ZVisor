#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Public/TimerManager.h"

#include "AIController.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/Constant.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "AICharacterController.generated.h"


class UBehaviorTree;
class ABlasterCharacter;
class URapidFollowPath;
class UPredictPossibleMove;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Damage;
class UAISenseConfig_Hearing;
class UAISense_Damage;
class UAISense_Hearing;

/**
 * 
 */
UCLASS()
class BLASTER_API AAICharacterController : public AAIController
{
	GENERATED_BODY()
	
public:
	friend ABlasterCharacter;
	AAICharacterController(FObjectInitializer const& ObjectInitializer);

	virtual void SetupCharacterController();
	virtual void Tick(float DeltaTime) override;
	virtual void SightOfPlayer(APawn* PlayerPawn);
	virtual void AttachWeapon();
	void SetLatestPath();


	/**
	* In sight / investigate
	*/
	void CheckValidTargetCharacterInSight();
	void SetInvestigate();
	void DisableInvestigate();
	void Investigate();

	/*
	* PPM
	*/
	void ActivePredictPossibleMove();
	void DisablePredictPossibleCooldown();

	/*
	* Detected
	*/
	void ActiveDetected();
	void DisableDetected();

	/**
	* Reset State
	*/
	void ClearTargetCharacter();
	void ResetSightSeeingAfterKill();

	/**
	* Suspect
	*/
	void IncreaseSuspectTimer();
	void DecreaseSuspectTimer();
	void ResetSuspectTimer();
	void InstantSuspectTimer();


	void DisableGettingHit(); //unavailable 
	
	/**
	* Blueprint - Event
	*/
	UFUNCTION()
	void ReportDamage(AActor* DamagedActor, float DamageAmount, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
	void ReportDamageInBlueprint(AActor* DamagedActor, float DamageAmount, AActor* DamageCauser);
	
	UFUNCTION(BlueprintCallable)
	void CharacterReceivedDamage(FVector LastLocationDamaged);

	/**
	* Distance From Character
	*/
	void SetDistanceFromCharacter();

	/**
	* Sight Seeing
	*/
	void SetSightSeeingPlayer(bool bActive);
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* CharacterPerceptionComponent;

	UPROPERTY(BlueprintReadOnly)
	ABlasterCharacter* CurrentAICharacter;

private:

	UPROPERTY(VisibleDefaultsOnly)
	ABlasterCharacter* TargetCharacter;
	FVector TargetCharacterLastLocation;
	
	UPROPERTY(EditAnywhere)
	bool bRandomLocation = false;

	/**
	* RFP
	*/
	UPROPERTY(EditAnywhere)
	URapidFollowPath* RapidFollowPath;

	UPROPERTY(VisibleAnywhere)
	UPredictPossibleMove* PredictPossibleMove;

	UPROPERTY(EditAnywhere)
	bool bPredictPossibleMove = false;

	/**
	* Perception
	*/
	UPROPERTY(EditAnywhere)
	UAIPerceptionStimuliSourceComponent* StimulusSource;

	UPROPERTY(EditAnywhere)
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditAnywhere)
	UAISenseConfig_Damage* DamageConfig;

	UPROPERTY(EditAnywhere)
	UAISenseConfig_Hearing* HearingConfig;

	/**
	* these two cannot work as 5.5 for implementation of UAISense
	* We will update this pointer later
	*/
	UPROPERTY(EditAnywhere)
	UAISense_Damage* DamageSense;

	UPROPERTY(EditAnywhere)
	UAISense_Hearing* HearingSense;


	/**
	* Setup
	*/
	void SetupPerception();
	void SetupCharacterSense();
	void SetupStimulusSource();
	void SetupRapidFollowPath();

	/**
	* Event Dynamic
	*/
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);


	/**
	* Variable
	*/
	bool bValidTargetCharacterInSight = false;

	float InvestigateTimer = 10.f;
	bool bPlayerInvestigate = false;	

	bool bTargetDetected = false;

	FTimerHandle InvestTimerHandle;
	FTimerHandle PPMTimerHandle;
	FTimerHandle PPMCooldownHandle;
	FTimerHandle SuspectHandle;

	float SuspectTimer = 0.f;
	bool bStartSuspect = false;
	bool bEndSuspect = false;

	float DistanceFromCharacter = 0.f;
	bool bStopMovement = false;

	bool bSightSeeingPlayer = false;
 

public:

	/*
	* this variable will only valid for HUD of Player
	* "Being Detected" means the bar still in progress (0 < x < 100)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bPlayerBeingDetected = false;

	UFUNCTION(BlueprintCallable)
	float GetSuspectTimer();

	ABlasterCharacter* GetTargetCharacter();
	FVector GetTargetCharacterLocation();
	UPredictPossibleMove* GetPredictPossibleMove();
	bool IsSameTeam(AActor* TargetActor);


	//FORCEINLINE float GetSuspectTimer() const { return SuspectTimer; };

	FORCEINLINE bool IsMaxSuspectTimer() const {
		return SuspectTimer >= SUSPECT_MAX_TIMER;
	}

	FORCEINLINE bool IsMinSuspectTimer() const {
		return SuspectTimer <= SUSPECT_MIN_TIMER;
	}

};
