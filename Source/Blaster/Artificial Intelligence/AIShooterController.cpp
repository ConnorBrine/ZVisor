#include "AIShooterController.h"
#include "Kismet/GameplayStatics.h"

#include "Blaster/Artificial Intelligence/AICharacterController.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Weapon/Weapon.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardComponent.h"


void AAIShooterController::BeginPlay()
{
	Super::BeginPlay();

	//GetBlackboardComponent()->SetValueAsVector(TEXT("SpawnLocation"), GetPawn()->GetActorLocation());

	//if (bStartedEquippedWeapon)
	//{
	//	Character = Cast<ABlasterCharacter>(GetPawn());
	//	if (Character)
	//	{
	//		Character->SetAICharacterSetup();
	//		FTimerHandle TimerHandle;
	//		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAIShooterController::AttachWeapon, 1.f, false);
	//		AttachWeapon();
	//	}		
	//}
}

void AAIShooterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/*APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	SightOfPlayer(PlayerPawn);*/

}

void AAIShooterController::SightOfPlayer(APawn* PlayerPawn)
{
	CurrentAICharacter->SetSightOfPlayer(LineOfSightTo(PlayerPawn));
	if (CurrentAICharacter->IsSightOfPlayer())
	{

		//MoveToActor(PlayerPawn, 150.f);
		//SetFocus(PlayerPawn);
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());

	}
	else
	{
		//ClearFocus(EAIFocusPriority::Gameplay);
		//StopMovement();
		//Character->SetMovementAccelerating(false);
		
		GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
	}
}

void AAIShooterController::AttachWeapon()
{
	//Super::AttachWeapon();
}
