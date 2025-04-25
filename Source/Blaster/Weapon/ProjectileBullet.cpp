#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RocketMovementComponent.h"
#include "BulletMovementComponent.h"
#include "Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "PhysicsEngine/ShapeElem.h"
#include "PhysicsEngine/BodySetup.h"
#include "Blaster/Weapon/HitboxType.h"

AProjectileBullet::AProjectileBullet()
{
	ProjectileBulletMovementComponent = CreateDefaultSubobject<UBulletMovementComponent>(TEXT("RocketMovementComponent"));
	ProjectileBulletMovementComponent->bRotationFollowsVelocity = true;
	ProjectileBulletMovementComponent->SetIsReplicated(true);
	ProjectileBulletMovementComponent->InitialSpeed = DEFAULT_SPEED;
	ProjectileBulletMovementComponent->MaxSpeed = DEFAULT_SPEED;
}


void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{

			UE_LOG(LogTemp, Error, TEXT("%s"), *OwnerCharacter->GetName());
			UE_LOG(LogTemp, Error, TEXT("%s"), *this->GetName());
			UE_LOG(LogTemp, Error, TEXT("%s"), *OtherActor->GetName());
			ABlasterCharacter* DamageCharacter = Cast<ABlasterCharacter>(OtherActor);
			if (DamageCharacter)
			{
				AWeapon* OwnerWeapon2 = OwnerCharacter->GetEquippedWeapon();

				float DamageToCause = GetProjectileHitbox(DamageCharacter, OwnerCharacter, Hit); //default damage base

				UE_LOG(LogTemp, Error, TEXT("Damage num: %f"), DamageToCause);

				UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, OwnerController, this, UDamageType::StaticClass());
			}
		
		}
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}


float AProjectileBullet::GetProjectileHitbox(ABlasterCharacter* HitCharacter, ABlasterCharacter* CauserCharacter, FHitResult HitBone)
{
	float DamageToCause = CauserCharacter->GetEquippedWeapon()->GetDamageBody();
	AWeapon* OwnerWeapon = CauserCharacter->GetEquippedWeapon();
	if (OwnerWeapon)
	{
		FBodyInstance* CharacterBoneInstance = HitCharacter->GetMesh()->GetBodyInstance(HitBone.BoneName);
		if (CharacterBoneInstance)
		{
			UBodySetup* CharacterBoneBody = CharacterBoneInstance->GetBodySetup();
			if (CharacterBoneBody)
			{
				FKShapeElem* CharacterBoneHitbox = CharacterBoneBody->AggGeom.GetElement(0);
				//CharacterBoneBody->AggGeom.GetElement(0);
				if (CharacterBoneHitbox)
				{
					//TODO: Hash later
					FString Hitbox = CharacterBoneHitbox->GetName().ToString();

					if (Hitbox.Contains("leg"))
					{
						DamageToCause = OwnerWeapon->GetDamageLeg();
						OwnerWeapon->SetHitboxType(EHitboxType::EHT_Leg);
					
					}
					if (Hitbox.Contains("hand_l"))
					{
						DamageToCause = OwnerWeapon->GetDamageHand();
						OwnerWeapon->SetHitboxType(EHitboxType::EHT_HandLeft);
						
					}
					if (Hitbox.Contains("hand_r"))
					{
						DamageToCause = OwnerWeapon->GetDamageHand();
						OwnerWeapon->SetHitboxType(EHitboxType::EHT_HandRight);
						
					}
					if (Hitbox.Contains("body"))
					{
						DamageToCause = OwnerWeapon->GetDamageBody();
						OwnerWeapon->SetHitboxType(EHitboxType::EHT_Body);
						
					}
					if (Hitbox.Contains("neck"))
					{
						DamageToCause = OwnerWeapon->GetDamageNeck();
						OwnerWeapon->SetHitboxType(EHitboxType::EHT_Neck);
						
					}
					if (Hitbox.Contains("headshot"))
					{
						DamageToCause = OwnerWeapon->GetDamageHead();
						OwnerWeapon->SetHitboxType(EHitboxType::EHT_Head);
						
					}
					UE_LOG(LogTemp, Error, TEXT("Gun is Shot in %s with %f dmg"), *CharacterBoneHitbox->GetName().ToString(), DamageToCause);

				}
			}
		}
	}
	return DamageToCause;
}

float AProjectileBullet::GetInitialSpeed() const
{
	return ProjectileBulletMovementComponent ? 
		ProjectileBulletMovementComponent->InitialSpeed : 0.f;
}