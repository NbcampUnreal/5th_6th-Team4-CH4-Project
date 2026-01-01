// AFAttributeComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AFAttributeComponent.generated.h"

<<<<<<< Updated upstream
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
=======
#pragma region FowardDeclare
class UNiagaraComponent;
class UNiagaraSystem;
class AAFPlayerState;
struct FAFPlayerCharacterStatRow;

#pragma endregion

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShieldChangedDelegate, float /*NewShield*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangedDelegate, float, CurrValue, float, MaxValue);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
>>>>>>> Stashed changes
class AFO_API UAFAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

	// ===========================================
	// 0. 초기화 및 동기화
	// ===========================================
#pragma region InitializeSetting
public:
	UAFAttributeComponent();
	void InitializeAttributes(const FAFPlayerCharacterStatRow& StatRow);	// 캐릭터 생성 시 데이터 테이블 기반 초기화
	void SyncToPlayerState(); // 현재 수치를 PlayerState로 보고

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

<<<<<<< Updated upstream
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attribute")
	float MaxHealth = 300.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute")
	float Health = 300.f;

	//사망 상태 플래그
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute")
	bool bIsDead = false;

public:
	void ApplyDamage(float Damage, AController* InstigatedBy);

=======
#pragma endregion

	// ===========================================
	// 1. 속성 제어 
	// ===========================================
#pragma region ControlAttribute
public:
	void ApplyDamage(float Damage, AController* InstigatedBy);

	// 체력 마나 회복 및 소모
	void ApplyHealthChange(float Amount);
	void ApplyManaChange(float Amount);

>>>>>>> Stashed changes
protected:
	void HandleDeath(AController* InstigatedBy);

<<<<<<< Updated upstream


public:
	// Health 변경 시 PlayerState에 동기화 하는 함수
	void SyncHealthToPlayerState();


private:
	FTimerHandle HealthSyncTimerHandle;





	// Floating Damage
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_NotifyDamage(float Damage, FVector Location, AController* InstigatedBy, bool bIsCritical);

};
=======
	AAFPlayerState* GetPlayerState() const; 	// 내부 도우미: 소유자의 PlayerState 안전하게 가져오기

#pragma endregion

	// ===========================================
	// 2. 변수 및 Getter
	// ===========================================
#pragma region AttributeVariable

protected:
	float CurrentHealth;
	float MaxHealth;
	float CurrentMana;
	float MaxMana;
	float BaseWalkSpeed;
	float AttackPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	bool bIsDead = false;

public:
	float GetHealth() const { return CurrentHealth; }
	float GetMaxHealth() const { return MaxHealth; }
	float GetMana() const { return CurrentMana; }
	float GetMaxMana() const { return MaxMana; }
	float GetBaseWalkSpeed() const { return BaseWalkSpeed; }
	float GetAttackPower() const { return AttackPower; }

	UFUNCTION(BlueprintPure, Category = "AFO|Attribute")
	bool IsDead() const { return bIsDead; }

#pragma endregion

	// ===========================================
	// 3. 보호막 & 버프
	// ===========================================
#pragma region Shield&Buff
public:

	void AddShield(float Amount, float Duration);	// 보호막 추가 함수
	void ApplyAttackBuff(float Multiplier, float Duration);  // 공격 버프 적용
	float GetAttackMultiplier() const { return AttackMultiplier; }	// 현재 공격력 배수 반환 (데미지 계산 시 사용)

	UPROPERTY(ReplicatedUsing = OnRep_CurrentShield, BlueprintReadOnly, Category = "AF|Status")
	float CurrentShield;

	UFUNCTION()
	void OnRep_CurrentShield(); // 보호막 이펙트 On/Off 제어용
	float GetCurrentShield() const { return CurrentShield; }	// 현재 보호막 수치 가져오기
	FOnShieldChangedDelegate OnShieldChanged;	// UI나 이펙트 갱신을 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "AF|Events")
	FOnAttributeChangedDelegate OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "AF|Events")
	FOnAttributeChangedDelegate OnManaChanged;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Attribute")
	float AttackMultiplier = 1.0;	// 공격력 배수 (기본값 1.0)
	FTimerHandle ShieldTimerHandle;
	FTimerHandle AttackBuffTimerHandle;
	void OnShieldExpired(); // 지속시간 종료 시 호출
#pragma endregion

	// ===========================================
	// 4. 시각효과 (Notify, Aura)
	// ===========================================
#pragma region Notify&Aura
public:
	// 현재 활성화된 오오라 컴포넌트를 저장
	UPROPERTY()
	UNiagaraComponent* ActiveAuraComponent;

	// 오오라 생성/제거 함수 (서버에서 실행되어 멀티캐스트로 전파)
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyAura(UNiagaraSystem* AuraSystem, FLinearColor Color, float Duration);

protected:
	// Floating Damage
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_NotifyDamage(float Damage, FVector Location, AController* InstigatedBy, bool bIsCritical);
#pragma endregion

	// ===========================================
	// 5. 캐릭터 스탯 데이터
	// ===========================================
#pragma region StatData

	UPROPERTY()
	UDataTable* StatDataTable;

#pragma endregion
};


>>>>>>> Stashed changes
