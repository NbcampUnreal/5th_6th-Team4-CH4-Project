// AFAttributeComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AFAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnShieldChangedDelegate, float /*NewShield*/);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AFO_API UAFAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAFAttributeComponent();

protected:
	virtual void BeginPlay() override;

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

protected:
	// 사망 처리 분리
	void HandleDeath(AController* InstigatedBy);

public:
	// Health 변경 시 PlayerState에 동기화 하는 함수
	void SyncHealthToPlayerState();

private:
	FTimerHandle HealthSyncTimerHandle;

	// Floating Damage
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_NotifyDamage(float Damage, FVector Location, AController* InstigatedBy, bool bIsCritical);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



public:

	// 보호막 추가 함수 (서버에서 호출)
	void AddShield(float Amount, float Duration);

	UPROPERTY(ReplicatedUsing = OnRep_CurrentShield, BlueprintReadOnly, Category = "Status")
	float CurrentShield;

	UFUNCTION()
	void OnRep_CurrentShield(); // 보호막 이펙트 On/Off 제어용

	// 현재 보호막 수치 가져오기
	float GetCurrentShield() const { return CurrentShield; }

	// UI나 이펙트 갱신을 위한 델리게이트
	FOnShieldChangedDelegate OnShieldChanged;

private:
	FTimerHandle ShieldTimerHandle;
	void OnShieldExpired(); // 지속시간 종료 시 호출
};