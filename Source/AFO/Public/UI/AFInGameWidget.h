// AFInGameWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFInGameWidget.generated.h"

class UTextBlock;
class UProgressBar;
class AAFPlayerState;

UCLASS()
class AFO_API UAFInGameWidget : public UUserWidget
{
	GENERATED_BODY()

	// ===========================================
	// 0. 초기화 및 동기화
	// ===========================================
#pragma region InitializeSetting
protected:
	virtual void NativeConstruct() override;

#pragma endregion

	// ===========================================
	// 1. 델리게이트 핸들러
	// ===========================================
#pragma region DelegateHandler
	// 실시간 수치 (HP, MP) 업데이트 - 내 HUD와 팀 스코어보드 공용
	UFUNCTION()
	void HandleAttributeChanged(float CurHP, float MaxHP, float CurMP, float MaxMP, AAFPlayerState* TargetPS);

	// 점수 (Kill, Death) 업데이트
	UFUNCTION()
	void HandleScoreChanged(int32 Kills, int32 Deaths, AAFPlayerState* TargetPS);

	// 기본 정보 (팀, 이름, 사망여부 등) 업데이트
	UFUNCTION()
	void HandlePlayerInfoChanged(AAFPlayerState* PS);

	// 타이머 델리게이트 핸들러
	UFUNCTION()
	void UpdateGameTimerText(int32 NewTime);

	// 팀 총합 스코어 업데이트 핸들러
	UFUNCTION()
	void UpdateTeamTotalScore();
#pragma endregion

	// =============================
	// 2. 내부 로직 및 바인딩
	// =============================
#pragma region Binding

	void BindToPlayerState(AAFPlayerState* PS);

	UFUNCTION()
	void HandlePlayerArrayChanged(); // GameState로부터 호출됨

	// 이미 UI 바인딩이 완료된 PlayerState들을 저장 (중복 방지)
	UPROPERTY()
	TSet<class AAFPlayerState*> BoundPlayerStates;

	// 최대 인원수 (GameState 등에서 가져오도록 설정 가능)
	int32 MaxPlayerCount = 4;

#pragma endregion

	// =============================
	// 3. UMG 위젯 변수
	// =============================
#pragma region WidgetVariable
protected:

	// A. 플레이어 이름 (TextBlock)
	UPROPERTY(meta = (BindWidget)) class UTextBlock* RedPlayerName01;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* RedPlayerName02;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* BluePlayerName01;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* BluePlayerName02;

	// B. 플레이어 HP/MP (ProgressBar)
	UPROPERTY(meta = (BindWidget)) class UProgressBar* RedPlayer1HP;
	UPROPERTY(meta = (BindWidget)) class UProgressBar* RedPlayer2HP;
	UPROPERTY(meta = (BindWidget)) class UProgressBar* RedPlayer1MP;
	UPROPERTY(meta = (BindWidget)) class UProgressBar* RedPlayer2MP;

	UPROPERTY(meta = (BindWidget)) class UProgressBar* BluePlayer1HP;
	UPROPERTY(meta = (BindWidget)) class UProgressBar* BluePlayer2HP;
	UPROPERTY(meta = (BindWidget)) class UProgressBar* BluePlayer1MP;
	UPROPERTY(meta = (BindWidget)) class UProgressBar* BluePlayer2MP;

	// C. 플레이어 킬/데스 (TextBlock)
	UPROPERTY(meta = (BindWidget)) class UTextBlock* RedPlayer1Kill;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* RedPlayer1Death;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* RedPlayer2Kill;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* RedPlayer2Death;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* BluePlayer1Kill;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* BluePlayer1Death;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* BluePlayer2Kill;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* BluePlayer2Death;

	// D. 팀 총합 스코어 (TextBlock)
	UPROPERTY(meta = (BindWidget)) class UTextBlock* RedKillScore;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* BlueKillScore;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* RedDeathScore;
	UPROPERTY(meta = (BindWidget)) class UTextBlock* BlueDeathScore;

	// E. 자신의 캐릭터 HP/MP
		// [Healt, Mana Bar]
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PlayerHP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PlayerMP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PlayerHP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_PlayerMP;

	// F. 타이머
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GameTimer;



#pragma endregion

	// =============================
	// 4. 게임 결과 위젯
	// =============================
#pragma region ResultWidget

protected:
	UFUNCTION()
	void ShowGameResult();

	// 게임 승리 패배 결과 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI|Result")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Result")
	TSubclassOf<UUserWidget> LoseWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI|Result")
	TSubclassOf<UUserWidget> DrawWidgetClass;

#pragma endregion
};
