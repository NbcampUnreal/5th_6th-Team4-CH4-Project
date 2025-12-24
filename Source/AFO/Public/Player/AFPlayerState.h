// AFPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AFPlayerState.generated.h"

// 체력/마나 변경 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthManaChanged, float, CurrentValue, float, MaxValue, class AAFPlayerState*, ChangedPlayer);

// 킬/데스 변경 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatCountChanged, int32, NewValue, class AAFPlayerState*, ChangedPlayer);

UCLASS()
class AFO_API AAFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AAFPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//  ==============================
	//  복제변수
	// ==============================
protected:
	UPROPERTY(Replicated)
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth; 

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Mana")
	float MaxMana = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentMana)
	float CurrentMana;

	UPROPERTY(ReplicatedUsing = OnRep_KillCount)
	int32 KillCount;

	UPROPERTY(ReplicatedUsing = OnRep_DeathCount)
	int32 DeathCount;

	UPROPERTY(Replicated)
	uint8 TeamID;  	// 팀 정보 ( 0: RED, 1: BLUE)

	UPROPERTY(Replicated)
	uint8 TeamIndex; 	// 팀 내 인덱스

	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
	bool bIsDead = false;

//  ===============================
//  OnRep 함수
// ================================
	UFUNCTION()
	void OnRep_CurrentHealth();
	UFUNCTION()
	void OnRep_CurrentMana();
	UFUNCTION()
	void OnRep_KillCount();
	UFUNCTION()
	void OnRep_DeathCount();

	UFUNCTION()
	void OnRep_IsDead();

//  ===============================
//  Public API (Getter / Setter)
// ================================

public:
	// 델리게이트 이벤트
	FOnHealthManaChanged OnHealthChanged;
	FOnHealthManaChanged OnManaChanged;
	FOnStatCountChanged OnKillCountChanged;
	FOnStatCountChanged OnDeathCountChanged;


	// Getter
	float GetCurrentHealth() const { return CurrentHealth; }
	float GetMaxHealth() const { return MaxHealth; }
	float GetCurrentMana() const { return CurrentMana; }
	float GetMaxMana() const { return MaxMana; }
	int32 GetKillCount() const { return KillCount; }
	int32 GetDeathCount() const { return DeathCount; }
	uint8 GetTeamID() const { return TeamID; }
	uint8 GetTeamIndex() const { return TeamIndex; }

	bool IsDead() const { return bIsDead; }

	//Setter (서버전용)
	void SetHealth(float NewHealth, float NewMaxHealth);
	void SetMana(float NewMana, float NewMaxMana);
	void IncrementKillCount();
	void IncrementDeathCount();
	void SetTeamInfo(uint8 NewTeamID, uint8 NewTeamIndex);

	void SetDead(bool bNewDead);
	void ResetForRespawn();


	// 추가 함수

	void AddMana(float Amount);
	bool ConsumeMana(float Amount);


};
