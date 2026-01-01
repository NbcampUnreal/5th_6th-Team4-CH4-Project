// AFGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "AFGameState.generated.h"

// 델리게이트 선언: 시간이 변경될 때마다 UI에게 알림
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerChangedDelegate, int32, NewRemainingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerArrayChangedDelegate);

<<<<<<< Updated upstream
// GamePhase 정의
=======
// GamePhase
>>>>>>> Stashed changes
UENUM(BlueprintType)
enum class EAFGamePhase : uint8
{
	EAF_Title							UMETA(DisplayName = "Tilte"),
	EAF_Loading					UMETA(DisplayName = "Loading"),
	EAF_Lobby						UMETA(DisplayName = "Lobby"),
	EAF_CharacterSelect		UMETA(DisplayName = "CharacterSelect	"),
	EAF_InGame					UMETA(DisplayName = "InGame"),
	EAF_GameOver				UMETA(DisplayName = "GameOver"),
};



UCLASS()
class AFO_API AAFGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AAFGameState();

	UPROPERTY(BlueprintAssignable, Category = "Networking")
	FPlayerArrayChangedDelegate OnPlayerArrayChanged;

	// 1. ReplicatedUsing 변수 선언: PlayerState 복제 상태를 추적합니다.
	UPROPERTY(ReplicatedUsing = OnRep_TeamPlayerArray, VisibleAnywhere, Category = "Networking")
	TArray<class AAFPlayerState*> TeamPlayerStatesReplicated;

	// 2. RepNotify 함수 선언: 클라이언트에 PlayerState 배열이 복제 완료될 때마다 호출됩니다.
	UFUNCTION()
	void OnRep_TeamPlayerArray();

	// 3. AddPlayerState 오버라이드 선언 (옵션: AGameStateBase에 이미 선언되어 있을 수 있으나 명확히 선언)
	virtual void AddPlayerState(APlayerState* PlayerState) override;



	// UI가 구독할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "AFO|Events")
	FOnTimerChangedDelegate OnTimerChanged;

	// 스코어 (킬 수)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "AFO|Score")
	int32 TeamRedKillScore = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "AFO|Score")
	int32 TeamBlueKillScore = 0;

protected:


	// 서버 타이머 핸들
	FTimerHandle GameTimerHandle;

	// 현재 게임 단계
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "AFO | Phase")
	EAFGamePhase CurrentGamePhase = EAFGamePhase::EAF_Title;

private:
	// === Replication ===
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 시간이 변경되면 호출됨 (클라이언트 + 서버 수동 호출)
	UFUNCTION()
	void OnRep_RemainingTime();

public:

	// 남은 시간
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, BlueprintReadOnly, Category = "AFO|Time")
	int32 RemainingTimeSeconds = 300.f;

	// === Server Functions ===
	// 타이머 시작 (GameMode에서 호출)
	void StartGameTimer();

	// 1초마다 호출될 함수
	void UpdateTimer();

	// === Server Authority Functions (GameMode에서 사용) ===

	// 서버에서 남은 시간을 설정하는 함수
	void SetRemainingTime(int32 NewTime);

	// 서버에서 게임 단계를 변경하는 함수
	void SetGamePhase(EAFGamePhase NewPhase);


	// 남은 시간 & 현재 Phase 가져오기
	UFUNCTION(BlueprintPure, Category = "AFO|Time")
	int32 GetRemainingTimeSeconds() const { return RemainingTimeSeconds; }

	UFUNCTION(BlueprintPure, Category = "AFO|Phase")
	EAFGamePhase GetCurrentGamePhase() const { return CurrentGamePhase; }
};