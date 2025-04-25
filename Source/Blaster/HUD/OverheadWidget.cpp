#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blaster/Character/BlasterCharacter.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText) 
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
	SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::UpdatePlayerHealth(float Health, float MaxHealth)
{
	const float HealthPercent = Health / MaxHealth;
	HealthBar->SetPercent(HealthPercent);

}

void UOverheadWidget::ShowHealthBar(bool bShow)
{
	HealthBar->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UOverheadWidget::UpdatePlayerSuspect(float SuspectNum)
{
	SuspectBar->SetPercent(SuspectNum / SUSPECT_MAX_TIMER);
}

void UOverheadWidget::ShowSuspectBar(bool bShow)
{
	SuspectBar->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}


void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}
