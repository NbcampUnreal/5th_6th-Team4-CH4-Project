#pragma once

#include "CoreMinimal.h"
#include "Character/AFPlayerCharacter.h"
#include "AFMage.generated.h"

UCLASS()
class AFO_API AAFMage : public AAFPlayerCharacter
{
	GENERATED_BODY()

public:
	AAFMage();
	virtual void Tick(float DeltaTime) override;
	
<<<<<<< Updated upstream
	virtual void StartSprint(const FInputActionValue& Value) override;
	virtual void StopSprint(const FInputActionValue& Value) override;
	
	// 점프 차단
	virtual void Jump() override;
	virtual void StopJumping() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	bool bIsSprinting = false;

	virtual void Tick(float DeltaTime) override;

	// AimOffset 값
	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category="Aim")
	float AimAlpha;

protected:
	virtual void BeginPlay() override;
=======
protected:
	// Mage 전용 스킬 수치 (에디터에서 수정 가능)
	UPROPERTY(EditAnywhere, Category = "Mage|Skill E") // 보호막 수치
	float E_ShieldAmount = 60.f;

	UPROPERTY(EditAnywhere, Category = "Mage|Balance") // 보호막 지속 시간
	float E_Duration = 5.f;

	UPROPERTY(EditAnywhere, Category = "Mage|Passive")	// 일반 공격 마나 감소량
	float ManaBurnAmount = 10.f;
	
	UPROPERTY(EditAnywhere, Category="Mage|Heavy Attack|FX")
	float Heavy_TraceStartUp = 1500.f; // 위에서 떨어지는 느낌(트레이스 시작 높이)
	
	UPROPERTY(EditAnywhere, Category="Mage|Heavy Attack|FX")
	float Heavy_FxDelay = 0.35f; // 강공격 이펙트가 터지는 타이밍(몽타주 임팩트 프레임에 맞추기)
	
	bool bWasHeavyMontagePlaying = false;

	// 부모의 판정 함수 오버라이드
	virtual void HandleOnCheckHit() override;

	virtual void Jump() override; // 점프 차단
	virtual void StopJumping() override;

	void SpawnEffectAndDamage(TSubclassOf<AActor> EffectClass, FAFSkillInfo* Data);


	// Q 이펙트 1회 재생 가드
	bool bQFxPlayed = false;
	
	bool bHeavyFxPlayed = false;

	virtual void ServerRPC_SkillE_Implementation() override;

	// 보호막 부여 전용 함수 (내부 로직 분리)
	void ApplyShieldToAllies(float Radius, float Amount);

	// 보호막 이펙트 멀티캐스트
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayShieldEffect(AActor* TargetActor);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnEffect(TSubclassOf<AActor> Class, FVector Loc, FRotator Rot);


	// 에디터에서 설정할 이펙트 에셋
	UPROPERTY(EditAnywhere, Category = "Mage|Effects")
	TObjectPtr<UNiagaraSystem> ShieldEffect;

	UPROPERTY(EditAnywhere, Category = "Mage|Effects")
	TSubclassOf<AActor> SkillQEffectBP;

	UPROPERTY(EditAnywhere, Category = "Mage|Effects")
	TSubclassOf<AActor> HeavyAttackEffectBP;

	UPROPERTY(EditAnywhere, Category = "Mage|Effects")
	TObjectPtr<UNiagaraSystem> HeavyAttackEffectNS;
>>>>>>> Stashed changes
};
