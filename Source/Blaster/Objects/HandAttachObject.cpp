#include "HandAttachObject.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AHandAttachObject::AHandAttachObject()
{

	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ComponentBox"));
	SetRootComponent(CollisionBox);

	Mesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh1"));
	Mesh1->SetupAttachment(RootComponent);
	Mesh1->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh1->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	
	Mesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh2"));
	Mesh2->SetupAttachment(RootComponent);
	Mesh2->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh2->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

}

void AHandAttachObject::BeginPlay()
{
	Super::BeginPlay();

	Mesh1->SetVisibility(false);
	Mesh2->SetVisibility(false);

	OnActorBeginOverlap.AddDynamic(this, &AHandAttachObject::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AHandAttachObject::OnOverlapEnd);

	
}

void AHandAttachObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHandAttachObject::DestroyObject()
{
	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyHandle, 0.1f, AHandAttachObject::Destroy());
}

void AHandAttachObject::ShowFirstMesh(bool bActiveMesh1)
{
	Mesh1->SetVisibility(bActiveMesh1);
	Mesh2->SetVisibility(!bActiveMesh1);

	bFirstAppear = bActiveMesh1;
}

void AHandAttachObject::SetSimulation(bool bSimulation)
{
	if (!Mesh1) return;
	bFirstAppear ?
		Mesh1->SetSimulatePhysics(bSimulation) :
		Mesh2->SetSimulatePhysics(bSimulation);
}

void AHandAttachObject::DetachObject()
{
	if (!Mesh1) return;
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	bFirstAppear ?
		Mesh1->DetachFromComponent(DetachRules) :
		Mesh2->DetachFromComponent(DetachRules);
}

void AHandAttachObject::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	//SetLifeSpan(3.f);
	//Destroy();
}

void AHandAttachObject::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
}

