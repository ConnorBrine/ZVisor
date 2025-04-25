#include "MainMenu3D_HUD.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"

AMainMenu3D_HUD::AMainMenu3D_HUD()
{
	PrimaryActorTick.bCanEverTick = true;
	MenuWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MenuWidget"));
	MenuWidget->SetDrawSize(FVector2D(600, 300));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}


void AMainMenu3D_HUD::BeginPlay()
{
	Super::BeginPlay();
	
}


void AMainMenu3D_HUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainMenu3D_HUD::TransitionTo()
{
	SetActorEnableCollision(true);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		APlayerController* Controller = Cast<APlayerController>(PlayerPawn->GetController());
		if (Controller)
		{
			Controller->SetViewTargetWithBlend(Camera->GetOwner(), 2.f);
		}
	}
	
}

