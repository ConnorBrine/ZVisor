#include "AICharacterController.h"
#include "EngineUtils.h"
#include "TimerManager.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"

#include "RapidFollowPath/PredictPossibleMove.h"
#include "Blaster/BlasterTypes/Constant.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Artificial Intelligence/RapidFollowPath/RapidFollowPath.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Blaster/Objects/ScanObjectType.h"

AAICharacterController::AAICharacterController(FObjectInitializer const& ObjectInitializer)
{
	CharacterPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
	if (CharacterPerceptionComponent == nullptr) return;

	SetupPerception();
	SetupStimulusSource();

	PredictPossibleMove = CreateDefaultSubobject<UPredictPossibleMove>(TEXT("PredictPossibleMove"));
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));


}

void AAICharacterController::CharacterReceivedDamage(FVector LastLocationDamaged)
{
	InstantSuspectTimer();

	SetSightSeeingPlayer(true);

	GetBlackboardComponent()->SetValueAsBool(TEXT("IsValidTargetCharacterInSight"), true);

	TargetCharacterLastLocation = LastLocationDamaged;

	GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), TargetCharacterLastLocation);
	DrawDebugSphere(GetWorld(), TargetCharacterLastLocation, 12, 12, FColor::Yellow, false);
}

void AAICharacterController::SetDistanceFromCharacter()
{
	if (CurrentAICharacter->GetName().Contains("Herlobcho")) return;
	if (bSightSeeingPlayer == false) return;

	DistanceFromCharacter = -1.f; //set this as check for nothing in target

	GetBlackboardComponent()->SetValueAsBool(TEXT("IsStayBack"), false);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsStopMovement"), false);

	if (TargetCharacter == nullptr)
	{
		GetBlackboardComponent()->SetValueAsFloat(TEXT("DistanceFromCharacter"), DistanceFromCharacter);
		return;
	}
	
	//distance 
	DistanceFromCharacter =
		(TargetCharacter->GetActorLocation() - CurrentAICharacter->GetActorLocation()).Size();
	GetBlackboardComponent()->SetValueAsFloat(TEXT("DistanceFromCharacter"), DistanceFromCharacter);

	if (DistanceFromCharacter < DISTANCE_FROM_CHARACTER_MIN) //too close
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsStayBack"), true);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsStopMovement"), false);

		int32 MoveRight = FMath::RandRange(-1, 1);
		CurrentAICharacter->SetAIMovement(-1.f, MoveRight);
		
	}

	else if (DISTANCE_FROM_CHARACTER_MIN <= DistanceFromCharacter &&
		DistanceFromCharacter <= DISTANCE_FROM_CHARACTER_MAX) //close to target
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsStayBack"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsStopMovement"), true);

		int32 MoveForward = FMath::RandRange(-1, 1);
		int32 MoveRight = FMath::RandRange(-1, 1);

		CurrentAICharacter->SetAIMovement(MoveForward, MoveRight);

		if (TargetCharacter == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Black, FString("stop movement"));
		}
	}
	else //far from target
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsStayBack"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsStopMovement"), false);
	}
}


void AAICharacterController::BeginPlay()
{
	Super::BeginPlay();

	CharacterPerceptionComponent = GetPerceptionComponent();
	if (CharacterPerceptionComponent)
	{
		CharacterPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAICharacterController::OnTargetDetected);
	}

	SetupCharacterController();

}

void AAICharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CharacterPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAICharacterController::OnTargetDetected);

	SetupCharacterController();
}

void AAICharacterController::SetupCharacterController()
{
	if (AIBehaviorTree)
	{
		RunBehaviorTree(AIBehaviorTree);
	}

	GetBlackboardComponent()->SetValueAsBool(TEXT("IsPlayerInvestigate"), false);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsIdleState"), true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsRandomLocation"), bRandomLocation);

	CurrentAICharacter = Cast<ABlasterCharacter>(GetPawn());
	if (CurrentAICharacter)
	{
		CurrentAICharacter->SetAICharacterSetup();
		CurrentAICharacter->SetMovementAccelerating(true);
	}
}

void AAICharacterController::SetupPerception()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig)
	{
		SightConfig->SightRadius = 1700.f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->SetMaxAge(5.f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 1000.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		CharacterPerceptionComponent->ConfigureSense(*SightConfig);
	}

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));
	if (DamageConfig)
	{
		DamageConfig->SetMaxAge(5.f);
		//GetPerceptionComponent()->SetDominantSense(*DamageConfig->GetSenseImplementation());
		CharacterPerceptionComponent->ConfigureSense(*DamageConfig);
	}

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	if (HearingConfig)
	{
		HearingConfig->HearingRange = 1000.f;
		HearingConfig->SetMaxAge(5.f);
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		//GetPerceptionComponent()->SetDominantSense(*HearingConfig->GetSenseImplementation());
		CharacterPerceptionComponent->ConfigureSense(*HearingConfig);
	}

	CharacterPerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());

}

void AAICharacterController::SetupCharacterSense()
{
	//this function still in progressing
	if (CharacterPerceptionComponent)
	{
		//UAISenseConfig* SenseConfig;
		//SenseConfig = CharacterPerceptionComponent->GetSenseConfig(DamageConfig->GetSenseID());
		//DamageSense = Cast<UAISense_Damage>(SenseConfig->GetSenseImplementation());

		//SenseConfig = CharacterPerceptionComponent->GetSenseConfig(HearingConfig->GetSenseID());
		//HearingSense = Cast<UAISense_Hearing>(SenseConfig->GetSenseImplementation());
	}
}
void AAICharacterController::SetupStimulusSource()
{
	if (StimulusSource)
	{
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Damage>());
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());

		StimulusSource->RegisterWithPerceptionSystem();
	}

}

void AAICharacterController::AttachWeapon()
{
	if (CurrentAICharacter)
	{
		//prevent spawning more
		if (CurrentAICharacter->IsWeaponEquipped()) return;
		ABlasterGameMode* CharacterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if (CharacterGameMode)
		{
			CharacterGameMode->SpawnObjectAtLocation(CurrentAICharacter->GetActorLocation(), EScanObjectType::ESOT_Weapon);
			CurrentAICharacter->AutoPickupEquippedWeapon();
		}
	}
}

void AAICharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (TargetCharacter)
	{
		SetDistanceFromCharacter();

		if (TargetCharacter->IsElimmed())
		{
			ResetSightSeeingAfterKill();
		}	
	}

	/*
	* always return false in order check if valid Player 
	* and "Being Detected" value
	*/
	bPlayerBeingDetected = false; 


	if (bTargetDetected)
	{
		IncreaseSuspectTimer();
	}
	else 
	{
		DecreaseSuspectTimer();
	}
}

void AAICharacterController::IncreaseSuspectTimer()
{
	bEndSuspect = true;
	if (IsMaxSuspectTimer())
	{ 
		//lock if max value
		if (bStartSuspect == false)
		{
			//function using only one when max
			bStartSuspect = !bStartSuspect;
			ActiveDetected();
		}
		
		return;
	}

	//check if valid Player will show HUD
	if (TargetCharacter && TargetCharacter == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		bPlayerBeingDetected = true;
	}

	//instant detected if not player
	if (TargetCharacter && TargetCharacter != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		SuspectTimer = SUSPECT_MAX_TIMER;
	}

	SuspectTimer =  
		FMath::Clamp(
			SuspectTimer + SUSPECT_INCREASE_TIMER,
			SUSPECT_MIN_TIMER, SUSPECT_MAX_TIMER);

		CurrentAICharacter->UpdateSuspect();
}

void AAICharacterController::DecreaseSuspectTimer()
{
	SetSightSeeingPlayer(false);

	bStartSuspect = false;

	if(CurrentAICharacter) CurrentAICharacter->AIFire(false);

	if (bEndSuspect == false)
	{
		//function using only one when min
		bEndSuspect = !bEndSuspect;
		DisableDetected(); //instant restore into investing
	}

	if (IsMinSuspectTimer())
	{
		//lock if min value
		ClearTargetCharacter();
		return;
	}

	//check if valid Player will show HUD
	if (TargetCharacter && TargetCharacter == UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		bPlayerBeingDetected = true;
	}

	//instant detected if not player
	if (TargetCharacter && TargetCharacter != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		SuspectTimer = SUSPECT_MAX_TIMER;
	}

	SuspectTimer = FMath::Clamp(
		SuspectTimer - SUSPECT_INCREASE_TIMER * 2.f,
		SUSPECT_MIN_TIMER, SUSPECT_MAX_TIMER);

	CurrentAICharacter->UpdateSuspect();
}

void AAICharacterController::InstantSuspectTimer()
{
	SuspectTimer = SUSPECT_MAX_TIMER;
	CurrentAICharacter->UpdateSuspect();
}

void AAICharacterController::ResetSuspectTimer()
{
	SuspectTimer = 0.f;
}

void AAICharacterController::ActiveDetected()
{
	SetSightSeeingPlayer(true);

	GetWorld()->GetTimerManager().ClearTimer(InvestTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(PPMTimerHandle, this, &AAICharacterController::ActivePredictPossibleMove, 10.f, true, 10.f);
}

void AAICharacterController::DisableDetected()
{
	SetSightSeeingPlayer(false);;
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsValidTargetCharacterInSight"), false);

	CurrentAICharacter->AIFire(false);

	GetWorld()->GetTimerManager().ClearTimer(PPMTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PPMCooldownHandle);
	Investigate();
}


void AAICharacterController::SightOfPlayer(APawn* PlayerPawn)
{
}



void AAICharacterController::CheckValidTargetCharacterInSight()
{

}

void AAICharacterController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (CurrentAICharacter == nullptr && Actor == nullptr) return;

	if (IsSameTeam(Actor)) return;

	//target: CurrentTargetCharacter / Actor
	//owner: Character

	//DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), CurrentTargetCharacter->GetActorLocation(), FColor::Yellow, true);
	//TODO: Run for thw line from character to target location

	TargetCharacter = Cast<ABlasterCharacter>(Actor);

	bTargetDetected = Stimulus.WasSuccessfullySensed();
	
	TargetCharacterLastLocation = Actor->GetActorLocation();

	GetBlackboardComponent()->SetValueAsBool(TEXT("IsValidTargetCharacterInSight"), true);

	GetWorld()->GetTimerManager().ClearTimer(SuspectHandle);

	if (DamageConfig &&
		DamageConfig->GetSenseID() &&
		Stimulus.Type == DamageConfig->GetSenseID())
	{
		InstantSuspectTimer();
	}
	else //sight
	{
		SetInvestigate();
		/*GetWorld()->GetTimerManager().SetTimer(SuspectHandle, this, &AAICharacterController::SetInvestigate, 2.f, false);*/
	}
	

}

ABlasterCharacter* AAICharacterController::GetTargetCharacter()
{
	return TargetCharacter;
}

FVector AAICharacterController::GetTargetCharacterLocation()
{
	if (TargetCharacter == nullptr) return TargetCharacterLastLocation;

	TargetCharacterLastLocation = TargetCharacter->GetActorLocation();
	return TargetCharacterLastLocation;
}

UPredictPossibleMove* AAICharacterController::GetPredictPossibleMove()
{
	return PredictPossibleMove;
}

bool AAICharacterController::IsSameTeam(AActor* TargetActor)
{
	return TargetActor->GetClass()->GetName().Contains(CurrentAICharacter->GetClass()->GetName());
}

float AAICharacterController::GetSuspectTimer()
{
	return SuspectTimer;
}

void AAICharacterController::SetupRapidFollowPath()
{

	//RapidFollowPath = NewObject<URapidFollowPath>();
	if (RapidFollowPath)
	{
		RapidFollowPath->SetupAISystem();
	}
	

	//RapidFollowPath->UpdateAISystem();
}

void AAICharacterController::Investigate()
{
	SetInvestigate();
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsIdleState"), false);
	if (CurrentAICharacter)
	{
		CurrentAICharacter->GetNextIdlePathIndex();
		CurrentAICharacter->AIFire(false);
	}
}


void AAICharacterController::ClearTargetCharacter()
{
	TargetCharacter = nullptr;
}

void AAICharacterController::DisableGettingHit()
{
	if (CurrentAICharacter)
	{
		CurrentAICharacter->DisableGettingHit();
	}
}

void AAICharacterController::ReportDamage(AActor* DamagedActor, float DamageAmount, AActor* DamageCauser)
{
	DamageSense->ReportDamageEvent(this, DamagedActor, DamageCauser, DamageAmount, DamagedActor->GetActorLocation(), CurrentAICharacter->GetActorLocation());
}

void AAICharacterController::ActivePredictPossibleMove()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsActivePPM"), true);
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsCooldownPPM"), true);

	GetWorld()->GetTimerManager().SetTimer(PPMCooldownHandle, this, &AAICharacterController::DisablePredictPossibleCooldown, 3.f, false);
}

void AAICharacterController::DisablePredictPossibleCooldown()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsCooldownPPM"), false);
}

void AAICharacterController::ResetSightSeeingAfterKill()
{

	UE_LOG(LogTemp, Warning, TEXT("Controller Received Elim"));
	ClearTargetCharacter();

	SetSightSeeingPlayer(false);
	DisableInvestigate();
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsIdleState"), false);
	CurrentAICharacter->AIFire(false); //prevent shooting everywhere
	CurrentAICharacter->AIReload(); //force reload after kill
}

void AAICharacterController::SetInvestigate()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsValidTargetCharacterInSight"), false); //lock this to run exact node
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsPlayerInvestigate"), true);

	GetWorld()->GetTimerManager().SetTimer(InvestTimerHandle, this, &AAICharacterController::DisableInvestigate, 5.f, true);
	//TimerHandle.
}

void AAICharacterController::DisableInvestigate()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsPlayerInvestigate"), false);
}

void AAICharacterController::SetLatestPath()
{
	int32 LatestIndex = Blackboard->GetValueAsInt(TEXT("IdlePathIndex"));
	CurrentAICharacter->SetLastestPathStats(LatestIndex);
}

void AAICharacterController::SetSightSeeingPlayer(bool bActive)
{
	bSightSeeingPlayer = bActive;
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsSightSeeingPlayer"), bActive);
}


//Testing in Tick

//bool IsInvestigateState = GetBlackboardComponent()->GetValueAsBool(TEXT("IsPlayerInvestigate"));
//if (IsInvestigateState)
//{
//	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, FString("invest"));
//}
//if(GetWorld()->GetTimerManager().GetTimerRemaining(InvestTimerHandle) != -1.f)
//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::SanitizeFloat(GetWorld()->GetTimerManager().GetTimerRemaining(InvestTimerHandle)));