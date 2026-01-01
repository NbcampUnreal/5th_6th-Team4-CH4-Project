// AFWereWolf.h

#pragma once

#include "CoreMinimal.h"
#include "Character/AFPlayerCharacter.h"
#include "AFWereWolf.generated.h"

UCLASS()
class AFO_API AAFWereWolf : public AAFPlayerCharacter
{
	GENERATED_BODY()
	
public:
	AAFWereWolf();

	virtual void BeginPlay() override;

<<<<<<< Updated upstream
=======
	virtual void HandleOnCheckHit() override;

	virtual void ServerRPC_SkillQ_Implementation() override;
	virtual void ServerRPC_SkillE_Implementation() override;

>>>>>>> Stashed changes
protected:
	bool bIsBleedModeActive = false;
	FTimerHandle BleedModeTimerHandle;

	void EndBleedMode();
	void ResetSpeedBoost();

	UPROPERTY(EditAnywhere, Category = "AF|Stat")
	float SpeedBoostDuration = 15.f;

	UPROPERTY(EditAnywhere, Category = "AF|Stat")
	float SpeedBoostMutifly = 1.5f;

	UPROPERTY(EditAnywhere, Category = "AF|Stat")
	float OriginalSpeed = 300.f;

	FTimerHandle SpeedBoostTimerHandle;

};