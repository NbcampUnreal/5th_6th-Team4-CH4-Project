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

	// 패시브: 평타 피흡 (데미지 적용 시 호출)
	void ApplyLifeSteal();

	// Q: 이동속도 증가 
	UFUNCTION(Server, Reliable)
	void Server_ApplySpeedBoost(float Ratio, float Duration);

	// E: 출혈 데미지
	UFUNCTION(Server, Reliable)
	void Server_ApplyBleeding(AActor* Target, float DamagePerSec, float Duration);

private:
	FTimerHandle SpeedBoostTimer;
	FTimerHandle BleedingTimer;

	float OriginalWalkSpeed = 0.f;

	//내부 처리 함수
	void ResetSpeedBoost();

	void ApplyBleedingTick(AActor* Target, float DamagePerSec);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float WereWolfMoveSpeed = 200.f; 
};