// AFInGameWidget.cpp


#include "UI/AFInGameWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Game/AFGameState.h"
#include "Kismet/GameplayStatics.h"
#include "AFO/Public/Player/AFPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "AFO/Public/Components/AFAttributeComponent.h"


// ====================
// 1. 초기화 및 바인딩 진입점
// ====================

void UAFInGameWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

	if (GameTimer)
	{
		GameTimer->SetText(FText::FromString("Wait..."));
	}

	// GameState를 찾아서 델리게이트 구독
	AAFGameState* GameState = Cast<AAFGameState>(GetWorld()->GetGameState());
	if (IsValid(GameState))
	{
		// 1. 이미 진행 중인 시간이 있을 수 있으므로 현재 시간으로 즉시 업데이트
		UpdateGameTimerText(GameState->RemainingTimeSeconds);

		// 2. 앞으로 변할 때마다 호출되도록 바인딩
		GameState->OnTimerChanged.AddDynamic(this, &UAFInGameWidget::UpdateGameTimerText);

		GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &UAFInGameWidget::CheckAndInitializeUI, 0.5f, true);
	}


	
	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		if (AAFGameState* AFGS = Cast<AAFGameState>(GS))
		{
			// GameState의 RepNotify 이벤트에 바인딩
			AFGS->OnPlayerArrayChanged.AddDynamic(this, &UAFInGameWidget::HandlePlayerArrayChanged);

			// 처음 한 번은 즉시 초기화를 시도
			HandlePlayerArrayChanged();
		}
	}

	

}

void UAFInGameWidget::HandlePlayerArrayChanged()
{
	// AAFGameState의 RepNotify가 발생할 때마다 호출됨
	AGameStateBase* GS = GetWorld()->GetGameState();

	if (!GS) return;

	// 초기화가 안 되었고, 최소 플레이어 수 (2명)가 PlayerArray에 들어왔는지 확인
	if (!bTeamUIInitialized && GS->PlayerArray.Num() >= 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandlePlayerArrayChanged: Initializing Team UI with %d PlayerStates."), GS->PlayerArray.Num());

		// PlayerArray의 모든 PlayerState를 넘겨 InitializeTeamUI 실행
		InitializeTeamUI(GS->PlayerArray);

		// InitializeTeamUI가 완료되면 bTeamUIInitialized = true 로 설정되어야 합니다.
	}
	else if (bTeamUIInitialized)
	{
		// (옵션) 이미 초기화된 후라면, 현재 GameState를 기반으로 팀 스코어를 업데이트하거나 상태를 재확인하는 로직을 추가할 수 있습니다.
		// 예: UpdateTeamKillDeathScore(0, nullptr); 
	}
}


void UAFInGameWidget::CheckAndInitializeUI()
{
	if (bTeamUIInitialized)
	{
		GetWorld()->GetTimerManager().ClearTimer(InitTimerHandle);
		return;
	}

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (!GS)
	{
		return;
	}

	// 2v2 게임이라고 가정하고 최소 2명의 플레이어를 기다립니다.
	if (GS->PlayerArray.Num() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("WAITING FOR ALL PLAYERS: Found %d/%d PlayerStates."), GS->PlayerArray.Num(), 2);
		return;
	}

	// ==========================================================
	// ★★★ 핵심 수정: 모든 PlayerState의 팀 ID가 복제되었는지 확인 ★★★
	// ==========================================================
	bool bAllPlayerStatesReady = true;
	for (APlayerState* PS : GS->PlayerArray)
	{
		AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS);
		// TeamID가 초기값(보통 0)이 아닌 1 이상이거나, AFPS 캐스팅 자체가 실패하면 준비 안 됨
		// 혹은 TeamID가 0이나 1 (RED/BLUE)로 확실히 설정되었는지 확인
		if (!AFPS || (AFPS->GetTeamID() != 0 && AFPS->GetTeamID() != 1))
		{
			bAllPlayerStatesReady = false;
			// UE_LOG(LogTemp, Warning, TEXT("WAITING: PlayerState %s TeamID not yet replicated."), AFPS ? *AFPS->GetPlayerName() : TEXT("NULL"));
			break;
		}
	}

	if (!bAllPlayerStatesReady)
	{
		UE_LOG(LogTemp, Warning, TEXT("WAITING: PlayerStates found, but Team IDs are not fully replicated yet."));
		return;
	}

	// 모든 플레이어 (2명)가 PlayerArray에 있고, TeamID도 복제되었다면 초기화 실행
	InitializeTeamUI(GS->PlayerArray);

	// InitializeTeamUI 내부에서 bTeamUIInitialized = true 가 설정되어 다음 틱에 타이머가 Clear됩니다.
}



void UAFInGameWidget::InitializeTeamUI(TArray<APlayerState*> AllPlayerStates)
{
	// 1. 초기화 방지 플래그 체크 (위치 유지)
	if (bTeamUIInitialized)
	{
		return;
	}

	// 초기화 시작
	TeamPlayerStates.Empty();

	APlayerController* LocalPC = GetOwningPlayer();
	FString InstanceName = LocalPC ? LocalPC->GetName() : TEXT("UNKNOWN_CLIENT");

	UE_LOG(LogTemp, Warning, TEXT("[%s] Initializing Team UI with %d PlayerStates."), *InstanceName, AllPlayerStates.Num());

	for (APlayerState* PS : AllPlayerStates)
	{
		if (AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS))
		{
			// 팀 로그
			UE_LOG(LogTemp, Warning, TEXT("[%s] Processing Player: %s (Team %d, Index %d)"),
				*InstanceName, *AFPS->GetPlayerName(), AFPS->GetTeamID(), AFPS->GetTeamIndex());

			// TeamPlayerStates Map 채우기 (팀 스코어 계산에 사용)
			TeamPlayerStates.FindOrAdd(AFPS->GetTeamID()).Add(AFPS);

			// ==============
			// 1. 이름 설정
			// ==============
			FText PlayerNameText = FText::FromString(AFPS->GetPlayerName());

			if (AFPS->GetTeamID() == 0) // RED 팀
			{
				if (AFPS->GetTeamIndex() == 1 && RedPlayerName01) RedPlayerName01->SetText(PlayerNameText);
				if (AFPS->GetTeamIndex() == 2 && RedPlayerName02) RedPlayerName02->SetText(PlayerNameText);
			}
			else // BLUE 팀
			{
				if (AFPS->GetTeamIndex() == 1 && BluePlayerName01) BluePlayerName01->SetText(PlayerNameText);
				if (AFPS->GetTeamIndex() == 2 && BluePlayerName02) BluePlayerName02->SetText(PlayerNameText);
			}

			// ========================
			// 2. 델리게이트 바인딩 및 초기값 설정
			// ========================

			// -----------------------------------------------------------
			// A. 로컬 플레이어 전용 HUD (HP/MP)
			// -----------------------------------------------------------
			if (AFPS->GetOwner() == LocalPC)
			{
				AFPS->OnHealthChanged.RemoveDynamic(this, &UAFInGameWidget::UpdateMyHealthBar);
				AFPS->OnManaChanged.RemoveDynamic(this, &UAFInGameWidget::UpdateMyManaBar);
				AFPS->OnHealthChanged.AddDynamic(this, &UAFInGameWidget::UpdateMyHealthBar);
				AFPS->OnManaChanged.AddDynamic(this, &UAFInGameWidget::UpdateMyManaBar);

				// 초기값 설정
				UpdateMyHealthBar(AFPS->GetCurrentHealth(), AFPS->GetMaxHealth(), AFPS);
				UpdateMyManaBar(AFPS->GetCurrentMana(), AFPS->GetMaxMana(), AFPS);

				UE_LOG(LogTemp, Warning, TEXT("[%s] BIND: Local Player HUD Bound."), *InstanceName);
			}

			// -----------------------------------------------------------
			// B. 스코어보드 개별 상태 (모든 플레이어)
			// -----------------------------------------------------------

			// 바인딩 전 정리 (RemoveDynamic 유지)
			AFPS->OnHealthChanged.RemoveDynamic(this, &UAFInGameWidget::UpdatePlayerHealthBar);
			AFPS->OnManaChanged.RemoveDynamic(this, &UAFInGameWidget::UpdatePlayerManaBar);
			AFPS->OnKillCountChanged.RemoveDynamic(this, &UAFInGameWidget::UpdatePlayerKillCount);
			AFPS->OnDeathCountChanged.RemoveDynamic(this, &UAFInGameWidget::UpdatePlayerDeathCount);

			// 바인딩 (AddDynamic 유지)
			AFPS->OnHealthChanged.AddDynamic(this, &UAFInGameWidget::UpdatePlayerHealthBar);
			AFPS->OnManaChanged.AddDynamic(this, &UAFInGameWidget::UpdatePlayerManaBar);
			AFPS->OnKillCountChanged.AddDynamic(this, &UAFInGameWidget::UpdatePlayerKillCount);
			AFPS->OnDeathCountChanged.AddDynamic(this, &UAFInGameWidget::UpdatePlayerDeathCount);

			// 초기값 설정
			UpdatePlayerHealthBar(AFPS->GetCurrentHealth(), AFPS->GetMaxHealth(), AFPS); // HP 초기값 추가 (누락 가능성)
			UpdatePlayerManaBar(AFPS->GetCurrentMana(), AFPS->GetMaxMana(), AFPS); // MP 초기값 추가
			UpdatePlayerKillCount(AFPS->GetKillCount(), AFPS);
			UpdatePlayerDeathCount(AFPS->GetDeathCount(), AFPS);

			// -----------------------------------------------------------
			// C. 팀 총합 스코어 바인딩 (모든 플레이어)
			// -----------------------------------------------------------

			// 바인딩 전 정리 (RemoveDynamic 유지)
			AFPS->OnKillCountChanged.RemoveDynamic(this, &UAFInGameWidget::UpdateTeamKillDeathScore);
			AFPS->OnDeathCountChanged.RemoveDynamic(this, &UAFInGameWidget::UpdateTeamKillDeathScore);

			// 바인딩 (AddDynamic 유지)
			AFPS->OnKillCountChanged.AddDynamic(this, &UAFInGameWidget::UpdateTeamKillDeathScore);
			AFPS->OnDeathCountChanged.AddDynamic(this, &UAFInGameWidget::UpdateTeamKillDeathScore);

			// ★★★ 디버깅 로그 추가 ★★★
			UE_LOG(LogTemp, Warning, TEXT("[%s] BIND SUCCESS: PlayerState %s (Team %d) scoreboard bound."),
				*InstanceName, *AFPS->GetPlayerName(), AFPS->GetTeamID());
		}
	} // ★★★ for 루프 끝 ★★★

	// ==========================================================
	// 3. 루프 완료 후 최종 초기화 및 플래그 설정
	// ==========================================================

	// ★★★ 팀 총합 스코어 초기화 (루프 밖으로 이동)
	// Map을 사용하여 전체 재계산 유도
	UpdateTeamKillDeathScore(0, nullptr);

	// ★★★ 초기화 성공 플래그 설정 (루프 밖으로 이동 - 핵심 수정)
	bTeamUIInitialized = true;

	UE_LOG(LogTemp, Warning, TEXT("[%s] Team UI Initialization COMPLETE."), *InstanceName);

	if (AllPlayerStates.Num() >= 2)
	{
		bTeamUIInitialized = true;
		UpdateTeamKillDeathScore(0, nullptr);
		UE_LOG(LogTemp, Warning, TEXT("[%s] Team UI Initialization COMPLETE. Total bound: %d"), *InstanceName, AllPlayerStates.Num());
	}
	else
	{
		// 2명이 안 되면 타이머를 계속 유지하도록 CheckAndInitializeUI를 통해 재시도해야 합니다.
		UE_LOG(LogTemp, Warning, TEXT("[%s] Initialization INCOMPLETE. Waiting for more players."), *InstanceName);
	}
}


// ====================
// 2. 델리게이트 핸들러
// ====================








// 팀 스코어보드 핸들러
void UAFInGameWidget::UpdatePlayerHealthBar(float CurrentHealth, float MaxHealth, AAFPlayerState* TargetPS)
{
	if (!TargetPS || MaxHealth <= 0.f) return;
	float Percent = CurrentHealth / MaxHealth;

	UProgressBar* TargetHPBar = nullptr;

	if (TargetPS->GetTeamID() == 0) // RED 팀
	{
		TargetHPBar = (TargetPS->GetTeamIndex() == 1) ? RedPlayer1HP : RedPlayer2HP;

		// ★★★ RED 팀 포인터 유효성 확인 로그 ★★★
		if (!TargetHPBar)
		{
			// 이 로그가 특정 클라이언트에서만 출력되면 UMG 런타임 바인딩 실패 확정
			UE_LOG(LogTemp, Error, TEXT("FATAL_UMG_BINDING: Red HP Bar (Index %d) is NULL on client %s!"), TargetPS->GetTeamIndex(), *GetOwningPlayer()->GetName());
		}
	}
	else // BLUE 팀
	{
		TargetHPBar = (TargetPS->GetTeamIndex() == 1) ? BluePlayer1HP : BluePlayer2HP;
	}

	if (TargetHPBar)
	{
		TargetHPBar->SetPercent(Percent);
		// ★★★ 업데이트 성공 로그 ★★★
		UE_LOG(LogTemp, Warning, TEXT("SCOREBOARD UPDATE SUCCESS: Team %d Index %d HP %.2f"), TargetPS->GetTeamID(), TargetPS->GetTeamIndex(), CurrentHealth);
	}
	else
	{
		// ★★★ 업데이트 실패 (UMG 포인터 문제) 로그 ★★★
		UE_LOG(LogTemp, Warning, TEXT("SCOREBOARD UPDATE FAILED: Team %d Index %d. TargetHPBar is NULL."), TargetPS->GetTeamID(), TargetPS->GetTeamIndex());
	}
}







void UAFInGameWidget::UpdatePlayerManaBar(float CurrentMana, float MaxMana, AAFPlayerState* TargetPS)
{
	if (!TargetPS || MaxMana <= 0.f) return;
	float Percent = CurrentMana / MaxMana;

	UProgressBar* TargetMPBar = nullptr;

	if (TargetPS->GetTeamID() == 0) // RED 팀
	{
		TargetMPBar = (TargetPS->GetTeamIndex() == 1) ? RedPlayer1MP : RedPlayer2MP;
	}
	else // BLUE 팀
	{
		TargetMPBar = (TargetPS->GetTeamIndex() == 1) ? BluePlayer1MP : BluePlayer2MP;
	}

	if (TargetMPBar)
	{
		TargetMPBar->SetPercent(Percent);
	}
}

void UAFInGameWidget::UpdatePlayerKillCount(int32 NewKillCount, AAFPlayerState* TargetPS)
{
	if (!TargetPS) return;

	FText NewKillText = FText::AsNumber(NewKillCount);

	if (TargetPS->GetTeamID() == 0) // RED 팀
	{
		if (TargetPS->GetTeamIndex() == 1 && RedPlayer1Kill) RedPlayer1Kill->SetText(NewKillText);
		if (TargetPS->GetTeamIndex() == 2 && RedPlayer2Kill) RedPlayer2Kill->SetText(NewKillText);
	}
	else // BLUE 팀
	{
		if (TargetPS->GetTeamIndex() == 1 && BluePlayer1Kill) BluePlayer1Kill->SetText(NewKillText);
		if (TargetPS->GetTeamIndex() == 2 && BluePlayer2Kill) BluePlayer2Kill->SetText(NewKillText);
	}
}

void UAFInGameWidget::UpdatePlayerDeathCount(int32 NewDeathCount, AAFPlayerState* TargetPS)
{
	if (!TargetPS) return;

	FText NewDeathText = FText::AsNumber(NewDeathCount);

	if (TargetPS->GetTeamID() == 0) // RED 팀
	{
		if (TargetPS->GetTeamIndex() == 1 && RedPlayer1Death) RedPlayer1Death->SetText(NewDeathText);
		if (TargetPS->GetTeamIndex() == 2 && RedPlayer2Death) RedPlayer2Death->SetText(NewDeathText);
	}
	else // BLUE 팀
	{
		if (TargetPS->GetTeamIndex() == 1 && BluePlayer1Death) BluePlayer1Death->SetText(NewDeathText);
		if (TargetPS->GetTeamIndex() == 2 && BluePlayer2Death) BluePlayer2Death->SetText(NewDeathText);
	}
}

// 팀 총합 스코어 갱신 핸들러
void UAFInGameWidget::UpdateTeamKillDeathScore(int32 NewValue, AAFPlayerState* TargetPS)
{
	int32 RedTotalKills = 0;
	int32 RedTotalDeaths = 0;
	int32 BlueTotalKills = 0;
	int32 BlueTotalDeaths = 0;

	// Map을 순회하며 팀별 총합을 계산
	for (const auto& Elem : TeamPlayerStates)
	{
		uint8 TeamID = Elem.Key;
		const TArray<AAFPlayerState*>& TeamMembers = Elem.Value;

		for (const AAFPlayerState* MemberPS : TeamMembers)
		{
			if (!MemberPS) continue;

			if (TeamID == 0) // RED 팀
			{
				RedTotalKills += MemberPS->GetKillCount();
				RedTotalDeaths += MemberPS->GetDeathCount();
			}
			else // BLUE 팀
			{
				BlueTotalKills += MemberPS->GetKillCount();
				BlueTotalDeaths += MemberPS->GetDeathCount();
			}
		}
	}

	// UI 업데이트
	if (RedKillScore) RedKillScore->SetText(FText::AsNumber(RedTotalKills));
	if (RedDeathScore) RedDeathScore->SetText(FText::AsNumber(RedTotalDeaths));
	if (BlueKillScore) BlueKillScore->SetText(FText::AsNumber(BlueTotalKills));
	if (BlueDeathScore) BlueDeathScore->SetText(FText::AsNumber(BlueTotalDeaths));
}

// 자신 전용 델리게이트 핸들러
void UAFInGameWidget::UpdateMyHealthBar(float NewHealth, float MaxHealth, AAFPlayerState* TargetPS)
{
	// 1. ProgressBar (PlayerHP) 업데이트
	if (PlayerHP)
	{
		float HealthPercent = MaxHealth > 0.f ? (NewHealth / MaxHealth) : 0.f;
		PlayerHP->SetPercent(HealthPercent);
	}

	// 2. TextBlock (Text_PlayerHP) 업데이트
	if (Text_PlayerHP)
	{
		FText HealthText = FText::Format(
			NSLOCTEXT("HealthUI", "HealthFormat", "{0} / {1}"),
			FText::AsNumber(FMath::RoundToInt(NewHealth)),
			FText::AsNumber(FMath::RoundToInt(MaxHealth))
		);
		Text_PlayerHP->SetText(HealthText);
	}

	UE_LOG(LogTemp, Log, TEXT("Local UI Health Updated: %.2f / %.2f"), NewHealth, MaxHealth);
}

void UAFInGameWidget::UpdateMyManaBar(float NewMana, float MaxMana, AAFPlayerState* TargetPS)
{
	// 1. ProgressBar (PlayerMP) 업데이트
	if (PlayerMP)
	{
		float ManaPercent = MaxMana > 0.f ? (NewMana / MaxMana) : 0.f;
		PlayerMP->SetPercent(ManaPercent);
	}

	// 2. TextBlock (Text_PlayerMP) 업데이트
	if (Text_PlayerMP)
	{
		FText ManaText = FText::Format(
			NSLOCTEXT("ManaUI", "ManaFormat", "{0} / {1}"),
			FText::AsNumber(FMath::RoundToInt(NewMana)),
			FText::AsNumber(FMath::RoundToInt(MaxMana))
		);
		Text_PlayerMP->SetText(ManaText);
	}

	UE_LOG(LogTemp, Log, TEXT("Local UI Mana Updated: %.2f / %.2f"), NewMana, MaxMana);
}

// 타이머 델리게이트 핸들러
void UAFInGameWidget::UpdateGameTimerText(int32 NewTime)
{
	if (!IsValid(GameTimer)) return;

	// "MM:SS" 포맷으로 변환
	int32 Minutes = NewTime / 60;
	int32 Seconds = NewTime % 60;

	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	GameTimer->SetText(FText::FromString(TimeString));

	// 30초 미만일 때 빨간색으로 변경하는 연출
	if (NewTime <= 30)
	{
		GameTimer->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	}
}




// PlayerState를 찾았을 때 델리게이트를 바인딩하는 최종 함수
void UAFInGameWidget::BindToPlayerState(AAFPlayerState* PS)
{
	// 타이머가 작동 중이라면 중지
	if (PlayerStateCheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerStateCheckTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("!!! PlayerState Timer Cleared !!!"));
	}

	// 기존 델리게이트 해제 
	PS->OnHealthChanged.RemoveDynamic(this, &UAFInGameWidget::UpdateMyHealthBar);
	PS->OnManaChanged.RemoveDynamic(this, &UAFInGameWidget::UpdateMyManaBar);

	// 1. 델리게이트에 C++ 함수 바인딩 (자신 전용 핸들러)
	PS->OnHealthChanged.AddDynamic(this, &UAFInGameWidget::UpdateMyHealthBar);
	PS->OnManaChanged.AddDynamic(this, &UAFInGameWidget::UpdateMyManaBar);

	// 2. 초기 값 설정 (TargetPS 인자 추가)
	UpdateMyHealthBar(PS->GetCurrentHealth(), PS->GetMaxHealth(), PS);
	UpdateMyManaBar(PS->GetCurrentMana(), PS->GetMaxMana(), PS);

	UE_LOG(LogTemp, Warning, TEXT("!!! Local HUD SUCCESSFULLY Bound & Initialized via Timer !!!"));

	// 3. 모든 플레이어의 상태를 바인딩하기 위해 InitializeTeamUI 호출
	if (AGameStateBase* GS = GetWorld()->GetGameState())
	{
		InitializeTeamUI(GS->PlayerArray);
	}
}

// 타이머가 호출하는 반복 체크 함수
void UAFInGameWidget::CheckAndBindPlayerState()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		// 위젯 소유 컨트롤러가 사라졌다면 타이머 중지
		GetWorld()->GetTimerManager().ClearTimer(PlayerStateCheckTimerHandle);
		return;
	}

	if (AAFPlayerState* PS = PC->GetPlayerState<AAFPlayerState>())
	{
		// PlayerState를 찾으면 바인딩 함수 호출
		BindToPlayerState(PS);
	}
	else
	{
		// 아직 못 찾았으면 다음 주기까지 기다립니다.
		UE_LOG(LogTemp, Log, TEXT("Waiting for PlayerState..."));
	}
}





