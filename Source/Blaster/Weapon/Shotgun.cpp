#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("Muzzle");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		TMap<ABlasterCharacter*, float> HitMap;
		for (uint32 i = 0; i < NumberOfPellets; i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			//However, this only apply in Multiplayer, so the AI(bot) will use another ways
			ABlasterCharacter* VictimCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			
			if (VictimCharacter && HasAuthority() && InstigatorController)
			{

				if (HitMap.Contains(VictimCharacter)) HitMap[VictimCharacter] += GetHitbox(VictimCharacter, FireHit);
				else HitMap.Emplace(VictimCharacter, GetHitbox(VictimCharacter, FireHit));


				UE_LOG(LogTemp, Error, TEXT("Pellet %d shot in %s"), i, *FireHit.BoneName.ToString());
			}

			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-.5f, .5f)
				);
			}
		}
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && HasAuthority() && InstigatorController)
			{
				UGameplayStatics::ApplyDamage(
					HitPair.Key,
					HitPair.Value,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
				UE_LOG(LogTemp, Error, TEXT("Character already have %f dmg"), HitPair.Value);
			}
		}
	}
}

