#pragma once

#include "CoreMinimal.h"
#include "AFPlayerCharacter.h"
#include "AFDarkKnight.generated.h"

UCLASS()
class AFO_API AAFDarkKnight : public AAFPlayerCharacter
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// 패시브: 출혈 (1초마다 5씩)
	void ApplyPassiveBleed(AActor* Target);

	// E 스킬: 광역 회전 공격
	UFUNCTION(Server, Reliable)
	void UseSkillE();

	// Q 스킬: 버프 (20초 동안 HP/MP/공격력 5% 상승)
	UFUNCTION(Server, Reliable)
	void UseSkillQ();

private:
	FTimerHandle PassiveBleedTimer;
	FTimerHandle QBuffTimer;

	bool bIsQBuffActive = false;

	void EndQBuff();
};
