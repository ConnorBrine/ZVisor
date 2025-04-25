#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Blaster.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
}



void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer) 
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}
	if (HasAuthority())
	{
		//all projectile will active destroy OnHit, except Hook Projectile
		if (bHookProjectile == false)
		{
			CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
		}
		
		CollisionBox->IgnoreActorWhenMoving(Owner, true);
	}
}


void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	if (OtherActor == GetOwner())
	{
		return;
	}
	
	Destroy();
}

void AProjectile::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
}
/**
* This will apply with  Explosive Projectile(Grenade, Rocket, Hook Projectile)
*/
void AProjectile::ExplodeDamage()
{
	UE_LOG(LogTemp, Warning, TEXT("Start"));
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("It's true"));
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UE_LOG(LogTemp, Warning, TEXT("Created Rocket"));

			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this, //World context object
				ExplosiveDamage, //BaseDamage
				MinimumDamage, 
				GetActorLocation(), 
				DamageInnerRadius, 
				DamageOuterRadius, 
				DamageFalloff, 
				UDamageType::StaticClass(), //DamageTypeClass
				TArray<AActor*>(), //IgnoreActors
				this, //DamageCauser
				FiringController //InstigatorController
			);

		}
	}
}


void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AProjectile::DestroyTimerFinished,
		DestroyTime
	);
}

void AProjectile::DestroyTimerFinished()
{
	Destroy();
}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}



