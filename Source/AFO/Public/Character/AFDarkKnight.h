#pragma once

#include "CoreMinimal.h"
#include "AFPlayerCharacter.h"
#include "AFDarkKnight.generated.h"

class UAnimMontage;

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

protected:
	// ===== Q 연출 (에디터에서 세팅) =====
	UPROPERTY(EditAnywhere, Category="DarkKnight|Skill Q")
	TObjectPtr<UAnimMontage> SkillQChargeMontage = nullptr;

	// 여기!! 나이아가라가 아니라 "BP 액터 클래스"를 넣을거임
	UPROPERTY(EditAnywhere, Category="DarkKnight|Skill Q|VFX")
	TSubclassOf<AActor> QChargeFXBP;

	UPROPERTY(EditAnywhere, Category="DarkKnight|Skill Q|VFX")
	FName QChargeAttachSocket = NAME_None;

	UPROPERTY(EditAnywhere, Category="DarkKnight|Skill Q|VFX")
	FVector QChargeFXLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category="DarkKnight|Skill Q|VFX")
	FRotator QChargeFXRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category="DarkKnight|Skill Q|Timing")
	float QChargeFxDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category="DarkKnight|Skill Q|Buff")
	float QBuffDuration = 20.f;

	UPROPERTY(EditAnywhere, Category="DarkKnight|Skill Q|Buff")
	float QBuffMultiplier = 1.05f;

private:
	FTimerHandle QBuffTimer;
	FTimerHandle QChargeDelayTimer;

	bool bIsQBuffActive = false;
	bool bIsQCharging = false;

	UPROPERTY(Transient)
	TObjectPtr<AActor> QChargeFXActor = nullptr;

	void ApplyQBuff_Server();
	void EndQBuff();

	// 핵심: 부모 입력이 부르는 RPC를 여기서 오버라이드해야 "실행됨"
	virtual void ServerRPC_SkillQ_Implementation() override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayQChargeMontage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartQChargeFX();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopQChargeFX();
};
