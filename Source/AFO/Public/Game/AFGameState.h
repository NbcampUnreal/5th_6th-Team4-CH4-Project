// AFGameState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "AFGameState.generated.h"

// GamePhase 정의
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
	// 스코어 (킬 수)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "AFO|Score")
	int32 TeamRedKillScore = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "AFO|Score")
	int32 TeamBlueKillScore = 0;

protected:
	// 남은 시간
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, BlueprintReadOnly, Category = "AFO|Time")
	int32 RemainingTimeSeconds = 0.f;

	// 현재 게임 단계
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "AFO | Phase")
	EAFGamePhase CurrentGamePhase = EAFGamePhase::EAF_Title;

private:
	UFUNCTION()
	void OnRep_RemainingTime();

public:
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
