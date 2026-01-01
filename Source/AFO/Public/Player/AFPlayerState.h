// AFPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AFPlayerState.generated.h"

<<<<<<< Updated upstream
// 체력/마나 변경 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthManaChanged, float, CurrentValue, float, MaxValue, class AAFPlayerState*, ChangedPlayer);
=======
#pragma region Delegate

// 1. 실시간 수치 (HP, MP)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnAttributeChanged, float, CurHP, float, MaxHP, float, CurMP, float, MaxMP, class AAFPlayerState*, TargetPS);
// 2. 게임 전적 (Kill, Death)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnScoreChanged, int32, Kills, int32, Deaths, class AAFPlayerState*, TargetPS);
// 3. 기본 정보 (팀, 이름, 캐릭터)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerInfoChanged, class AAFPlayerState*, PS);
>>>>>>> Stashed changes

#pragma endregion

<<<<<<< Updated upstream
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamInfoChanged, AAFPlayerState*, ChangedPlayer);

=======
>>>>>>> Stashed changes
UCLASS()
class AFO_API AAFPlayerState : public APlayerState
{
	GENERATED_BODY()

	// ===========================================
	// 0. 초기화 및 동기화
	// ===========================================
#pragma region InitializeSetting
public:
	AAFPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OverrideWith(APlayerState* PlayerState) override;
<<<<<<< Updated upstream

	UPROPERTY(BlueprintAssignable)
	FOnTeamInfoChanged OnTeamInfoChanged;

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

	UPROPERTY(ReplicatedUsing = OnRep_TeamInfo)
	uint8 TeamID;

	UPROPERTY(ReplicatedUsing = OnRep_TeamInfo)
	uint8 TeamIndex;


	UFUNCTION()
	void OnRep_TeamInfo();

	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
	bool bIsDead = false;

	UPROPERTY(ReplicatedUsing = OnRep_SelectedCharacter) 
	uint8 SelectedCharacterId = 255;

	UPROPERTY(ReplicatedUsing = OnRep_Ready)
	bool bReady = false;

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

	UFUNCTION() 
	void OnRep_SelectedCharacter();

	UFUNCTION()
	void OnRep_Ready();
//  ===============================
//  Public API (Getter / Setter)
// ================================
=======


#pragma endregion

	// ===========================================
	// 1. 복제변수 및 OnRep함수
	// ===========================================
#pragma region ReplicatedVariable
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Attributes) float CurrentHealth;
	UPROPERTY(ReplicatedUsing = OnRep_Attributes) float MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_Attributes) float CurrentMana;
	UPROPERTY(ReplicatedUsing = OnRep_Attributes) float MaxMana;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerScore) int32 KillCount;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerScore) int32 DeathCount;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo) uint8 TeamID;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo)uint8 TeamIndex;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo) uint8 SelectedCharacterId = 255;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo) FText SelectedCharacterName;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo) bool bReady = false;

	// --- OnRep 함수 (UI 알림용) ---
protected:
	UFUNCTION() void OnRep_Attributes(); // HP, MP 변화 시 호출
	UFUNCTION() void OnRep_PlayerScore();      // Kill, Death 변화 시 호출
	UFUNCTION() void OnRep_PlayerInfo(); // 팀, 캐릭터 정보 변화 시 호출
	virtual void OnRep_PlayerName() override;

#pragma endregion
>>>>>>> Stashed changes

	// ===========================================
	// 2. 델리게이트
	// ===========================================
#pragma region ReplicatedVariable
public:
	// UI에서 바인딩할 통합 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "AF|Events") FOnAttributeChanged OnAttributeChanged;
	UPROPERTY(BlueprintAssignable, Category = "AF|Events") FOnScoreChanged OnScoreChanged;
	UPROPERTY(BlueprintAssignable, Category = "AF|Events") FOnPlayerInfoChanged OnPlayerInfoChanged;

#pragma endregion

<<<<<<< Updated upstream
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

	bool HasSelectedCharacter() const { return SelectedCharacterId != 255; }
	uint8 GetSelectedCharacterId() const { return SelectedCharacterId; }
	bool IsReady() const { return bReady; }

	//Setter (서버전용)
=======
	// ===========================================
	// 3. Getter&Setter
	// ===========================================
#pragma region Getter&Setter
public:
	// [Setter - 서버 전용]
>>>>>>> Stashed changes
	void SetHealth(float NewHealth, float NewMaxHealth);
	void SetMana(float NewMana, float NewMaxMana);
	void AddKill();
	void AddDeath();


	void SetTeamInfo(uint8 NewTeamID, uint8 NewTeamIndex);
	void SetSelectedCharacter_Server(uint8 InId);
	void ResetLobbySelection_Server();
	void SetReady_Server(bool bNewReady);
	void ResetForRespawn();

	// [Getter]
	uint8 GetSelectedCharacterId() const { return SelectedCharacterId; }
	uint8 GetTeamID() const { return TeamID; }
	uint8 GetTeamIndex() const { return TeamIndex; }
	int32 GetKillCount() const { return KillCount; }  
	int32 GetDeathCount() const { return DeathCount; } 
	float GetHealthPercent() const { return (MaxHealth > 0.f) ? CurrentHealth / MaxHealth : 0.f; }
	bool IsReady() const { return bReady; }
	bool HasSelectedCharacter() const { return SelectedCharacterId != 255; }
	bool IsDead() const;
	UFUNCTION(BlueprintPure, Category = "AFO|Character") 	FText GetSelectedCharacterName() const;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AFO|Character") 	TArray<FText> CharacterDisplayNames;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo)
	FString SelectedCharacterKey = TEXT("Unknown");
	FString GetSelectedCharacterKey() const { return SelectedCharacterKey; }
	void SetSelectedCharacterKey_Server(FString InKey);
#pragma endregion

<<<<<<< Updated upstream
	// 추가 함수

	void AddMana(float Amount);
	bool ConsumeMana(float Amount);

=======

// ===========================================
// 4. 전적 통계
// ===========================================
#pragma region GameResult
protected:
	// 전적 통계용
	UPROPERTY(ReplicatedUsing = OnRep_PlayerScore)  float TotalDamageDealt;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerScore)  float TotalDamageTaken;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerScore)  float TotalHealingDone;

public:
	// 통계 누적 함수 (서버 전용)
	void AddDamageDealt(float Damage) { if (HasAuthority()) TotalDamageDealt += Damage; }
	void AddDamageTaken(float Damage) { if (HasAuthority()) TotalDamageTaken += Damage; }
	void AddHealingDone(float Amount) { if (HasAuthority()) TotalHealingDone += Amount; }
>>>>>>> Stashed changes

	// Getter
	float GetTotalDamageDealt() const { return TotalDamageDealt; }
	float GetTotalDamageTaken() const { return TotalDamageTaken; }
	float GetTotalHealingDone() const { return TotalHealingDone; }
#pragma endregion
};