#include "MeleeWeapon.h"

#include "Components/BoxComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "Blaster/Character/BlasterCharacter.h"

AMeleeWeapon::AMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	MeleeDamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeDamageCollision"));
	MeleeDamageCollision->SetupAttachment(RootComponent, "root");
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}


void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("Tick"));
	//if (bAttackStart) 
	MeleeDetect();

}

void AMeleeWeapon::LightAttack()
{
	//APawn* OwnerPawn = Cast<APawn>(GetOwner());
	//if (OwnerPawn == nullptr) return;
	//AController* InstigatorController = OwnerPawn->GetController();


	if (true)
	{
		//ABlasterCharacter* VictimCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());

		////get damage by using box collision to detect the range
		//if (VictimCharacter && HasAuthority() && InstigatorController)
		//{

		//	float DamageToCause = GetHitbox(VictimCharacter, FireHit); //default damagae base

		//	UE_LOG(LogTemp, Error, TEXT("%s"), *OwnerPawn->GetName());
		//	UE_LOG(LogTemp, Error, TEXT("%s"), *this->GetName());
		//	UE_LOG(LogTemp, Error, TEXT("Damage num: %f"), DamageToCause);


		//	UE_LOG(LogTemp, Warning, TEXT("Victim damage begin"));
		//	UGameplayStatics::ApplyDamage(
		//		VictimCharacter,
		//		DamageToCause,
		//		InstigatorController,
		//		this,
		//		UDamageType::StaticClass()
		//	);
		//}
		//if (ImpactParticles)
		//{
		//	UGameplayStatics::SpawnEmitterAtLocation(
		//		GetWorld(),
		//		ImpactParticles,
		//		FireHit.ImpactPoint,
		//		FireHit.ImpactNormal.Rotation()
		//	);
		//}
		//if (HitSound)
		//{
		//	UGameplayStatics::PlaySoundAtLocation(
		//		this,
		//		HitSound,
		//		FireHit.ImpactPoint
		//	);
		//}
		//if (MuzzleFlash)
		//{
		//	UGameplayStatics::SpawnEmitterAtLocation(
		//		GetWorld(),
		//		MuzzleFlash,
		//		SocketTransform
		//	);
		//}
		//if (FireSound)
		//{
		//	UGameplayStatics::PlaySoundAtLocation(
		//		this,
		//		FireSound,
		//		GetActorLocation()
		//	);
		//}
	}
}

void AMeleeWeapon::HeavyAttack()
{

}


void AMeleeWeapon::AttackStart()
{
	SetActorTickEnabled(true);
	
}

void AMeleeWeapon::AttackEnd()
{
	SetActorTickEnabled(false);
}


float AMeleeWeapon::GetWeaponRange()
{
	return 0.0f;
}