#include "ProjectileHook.h"
#include "CableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "BulletMovementComponent.h"
#include "Blaster/Blaster.h"
#include "Blaster/Math/MathCalculation.h"

AProjectileHook::AProjectileHook()
{
	bHookProjectile = true;

	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	CollisionBox->SetRelativeLocation(FVector(0, 0, 0));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

	Rope = CreateDefaultSubobject<UCableComponent>(TEXT("Rope"));
	Rope->SetupAttachment(RootComponent);
	Rope->SetRelativeLocation(FVector(20.f, 0, 0));
	Rope->EndLocation = FVector(0, 0, 0);
	Rope->CableWidth = 10.f;
	Rope->bAttachStart = true;
	Rope->bAttachEnd = true;
	Rope->bEnableStiffness = true;
	Rope->CableWidth = 4.f;
	Rope->SolverIterations = 16.f;

	Explosive = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Explosive"));
	Explosive->SetupAttachment(RootComponent);
	Explosive->SetRelativeLocation(FVector(-50.f, 0, 0));
	Explosive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Explosive->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Explosive->SetSimulatePhysics(false);
	Explosive->SetEnableGravity(true);

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookMesh"));
	HookMesh->SetupAttachment(RootComponent);
	HookMesh->SetRelativeLocation(FVector(50.f, 0, 0));
	HookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HookMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//mass = 400.f

	HookMovementComponent = CreateDefaultSubobject<UBulletMovementComponent>(TEXT("HookMovementComponent"));
	HookMovementComponent->bRotationFollowsVelocity = true;
	HookMovementComponent->SetIsReplicated(true);
	HookMovementComponent->InitialSpeed = DEFAULT_SPEED;
	HookMovementComponent->MaxSpeed = DEFAULT_SPEED;

	PhysicsConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraintComponent"));
	PhysicsConstraintComponent->SetupAttachment(RootComponent);
	PhysicsConstraintComponent->ComponentName1.ComponentName = HookMesh->GetFName();
	PhysicsConstraintComponent->ComponentName2.ComponentName = Explosive->GetFName();

	Rope->AttachEndTo.ComponentProperty = Explosive->GetFName();

}

void AProjectileHook::BeginPlay()
{
	Super::BeginPlay();
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, FString("Begin Hook"));

	EnablePhysicsConstraint();
	CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileHook::ProjectileHookOnHit);
	Explosive->OnComponentHit.AddDynamic(this, &AProjectileHook::ExplosiveOnHit);

	FTimerHandle ExplosiveTimer;
	GetWorldTimerManager().SetTimer(ExplosiveTimer, this, &AProjectileHook::Explode, EXPLODE_TIMER, false);
	

}

/**
 * OnHit ProjectileHook then active CablePhysics
 * Cable Physics will finish if Explosive Collision with any Object
 * finally Destroy OnHit (Optional)
 */


void AProjectileHook::EnablePhysicsConstraint()
{

	PhysicsConstraintComponent->ConstraintActor1 = this;
	PhysicsConstraintComponent->ConstraintActor2 = this;
}

void AProjectileHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//assign tick with rope for explosive movement when active physics

}
							   
void AProjectileHook::HookOnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	//Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString("HitComp: ") + *HitComp->GetName());
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString("OtherActor: ") + *OtherActor->GetName());
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString("OtherComp: ") + *OtherComp->GetName());
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString("HitComp") + FString("Hook Hit"));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PhysicsConstraintComponent->SetWorldLocation(GetActorLocation());
	Explosive->SetSimulatePhysics(true);

}

void AProjectileHook::OnComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, *OverlappedComponent->GetName());
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, *OtherActor->GetName());
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString("Hook Hit"));
	PhysicsConstraintComponent->SetWorldLocation(GetActorLocation());
	Explosive->SetSimulatePhysics(true);

}

void AProjectileHook::ExplosiveOnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner() || OtherActor == this)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Hit SELF"));
		UE_LOG(LogTemp, Warning, TEXT("Hit SELF"));
		return;
	}
	USceneComponent* FinalHitLoc = Cast<USceneComponent>(Explosive);
	DrawDebugSphere(GetWorld(), FinalHitLoc->K2_GetComponentLocation(), 12.f, 12, FColor::Green, true);
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Active Explosive"));
	

	Explode();
}


void AProjectileHook::Explode()
{
	ExplodeDamage();

	StartDestroyTimer();

	Destroy();
}


void AProjectileHook::ProjectileHookOnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	//rotation check
	UE_LOG(LogTemp, Warning, TEXT("ActorMap: %f, %f, %f"),
		OtherActor->GetActorRotation().Pitch ,
		OtherActor->GetActorRotation().Yaw,
		OtherActor->GetActorRotation().Roll);
	UE_LOG(LogTemp, Warning, TEXT("Comp: %f, %f, %f"),
		HitComp->GetComponentRotation().Pitch,
		HitComp->GetComponentRotation().Yaw,
		HitComp->GetComponentRotation().Roll);



	if (OtherActor == GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit SELF"));
		return;
	}

	//calculator
	//FVector ClosestLocation;
	FString Result;


	
	if (IsHookCloserWithGivenVector(OtherActor->GetActorLocation()))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString("Active Hook"));
		bHit = true;
		Explosive->SetSimulatePhysics(true);
	}

	

	
}

double AProjectileHook::GetDistance(FVector GivenA, FVector GivenB) const
{
	return FMath::Sqrt(
		FMath::Pow(GivenA.X - GivenB.X, 2) +
		FMath::Pow(GivenA.Y - GivenB.Y, 2) + 
		FMath::Pow(GivenA.Z - GivenB.Z, 2) 
	);
}

bool AProjectileHook::IsHookCloserWithGivenVector(FVector GivenVector)
{
	return UMathCalculation::GetDistance(
		HookMesh->GetComponentLocation(),
		GivenVector
	) < UMathCalculation::GetDistance(
		Explosive->GetComponentLocation(), 
		GivenVector
	);
}


float AProjectileHook::GetInitialSpeed() const
{
	return HookMovementComponent ?
		HookMovementComponent->InitialSpeed : 0.f;
}

float AProjectileHook::GetBaseDamage() const
{
	return ExplosiveDamage;
}
