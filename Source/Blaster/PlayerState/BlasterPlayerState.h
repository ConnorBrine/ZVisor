#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;
class UPathChapter;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ABlasterPlayerState();
	virtual void OnRep_Score() override;
	void AddToKills(float KillsAmount);
	virtual void BeginPlay() override;

	void ReloadPathChapterDataTable();
	UPathChapter* GetPathChapter();
protected:

private:
	ABlasterCharacter* Character;
	ABlasterPlayerController* Controller;

	//Add UPROPERTY for replicated later
	UPathChapter* CurrentPathChapter;

};
