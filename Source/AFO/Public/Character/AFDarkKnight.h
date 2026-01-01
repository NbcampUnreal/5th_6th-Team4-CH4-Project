#pragma once

#include "CoreMinimal.h"
#include "Character/AFPlayerCharacter.h"
#include "AFDarkKnight.generated.h"

UCLASS()
class AFO_API AAFDarkKnight : public AAFPlayerCharacter
{
	GENERATED_BODY()
<<<<<<< Updated upstream
	
public:
	virtual void StartSprint(const FInputActionValue& Value) override;
	virtual void StopSprint(const FInputActionValue& Value) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	bool bIsSprinting = false;
=======

public:
	AAFDarkKnight();

protected:

	virtual void HandleOnCheckHit() override;

	// Q 버프 관련 수치
	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q|Balance")
	float QBuff_HealRatio = 0.2f;      // 20% 회복

	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q|Balance")
	float QBuff_DamageRatio = 1.2f;    // 공격력 20% 증가

	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q|Balance")
	float QBuff_SpeedRatio = 1.2f;     // 이동속도 20% 증가

	// 패시브 출혈 설정
	UPROPERTY(EditAnywhere, Category = "DarkKnight|Passive")
	float PassiveBleedProbability = 0.2f; // 20% 확률

	// ===== 이펙트 로직  =====
	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q")
	TObjectPtr<UAnimMontage> SkillQChargeMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q|VFX")
	TSubclassOf<AActor> QChargeFXBP;

	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q|VFX")
	FName QChargeAttachSocket = NAME_None;

	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q|VFX")
	FVector QChargeFXLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q|VFX")
	FRotator QChargeFXRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q|Timing")
	float QChargeFxDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category = "DarkKnight|Skill Q|Buff")
	float QBuffDuration = 10.f;

private:
	FTimerHandle QBuffTimer;
	FTimerHandle QChargeDelayTimer;

	bool bIsQBuffActive = false;
	bool bIsQCharging = false;

	UPROPERTY(Transient)
	TObjectPtr<AActor> QChargeFXActor = nullptr;

	void ApplyQBuff_Server();
	void EndQBuff();


	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayQChargeMontage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartQChargeFX();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopQChargeFX();
>>>>>>> Stashed changes
};
