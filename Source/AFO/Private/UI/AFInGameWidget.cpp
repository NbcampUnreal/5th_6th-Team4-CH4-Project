// AFInGameWidget.cpp


#include "UI/AFInGameWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Game/AFGameState.h"
#include "AFO/Public/Player/AFPlayerState.h"
#include "Player/AFPlayerController.h"
#include "Components/AFAttributeComponent.h"


	// ===========================================
	// 0. 초기화 및 동기화
	// ===========================================
#pragma region InitializeSetting
void UAFInGameWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();

<<<<<<< Updated upstream
	if (GameTimer)
	{
		GameTimer->SetText(FText::FromString("Wait..."));
=======
	// 1. 타이머 초기화
	if (GameTimer) GameTimer->SetText(FText::FromString(TEXT("00:00")));

	AAFGameState* GS = GetWorld()->GetGameState<AAFGameState>();
	if (GS)
	{
		// 2. GameState 관련 델리게이트 연결
		GS->OnTimerChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdateGameTimerText);
		GS->OnPlayerArrayChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdateTeamTotalScore);

		// 점수판 델리게이트가 GameState에 있다면 여기에 추가 (예: OnTeamScoreChanged)
		// 현재는 HandleScoreChanged 내부나 별도 함수에서 처리

		// 3. 이미 존재하는 플레이어 바인딩 시도
		HandlePlayerArrayChanged();
>>>>>>> Stashed changes
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

#pragma endregion

// ===========================================
// 1. 델리게이트 핸들러
// ===========================================
#pragma region DelegateHandler
void UAFInGameWidget::HandleAttributeChanged(float CurHP, float MaxHP, float CurMP, float MaxMP, AAFPlayerState* TargetPS)
{
	if (!IsValid(TargetPS)) return;

<<<<<<< Updated upstream
	// 현재 접속 중인 모든 플레이어를 대상으로 초기화 시도
	// bTeamUIInitialized 체크를 제거하고 내부에서 개별 플레이어별로 체크합니다.
	InitializeTeamUI(GS->PlayerArray);
}

void UAFInGameWidget::CheckAndInitializeUI()
{
	if (bTeamUIInitialized)
=======
	// A. 내 HUD 업데이트 (로컬 플레이어인 경우)
	if (TargetPS->GetOwner() == GetOwningPlayer())
>>>>>>> Stashed changes
	{
		if (PlayerHP) PlayerHP->SetPercent(MaxHP > 0.f ? CurHP / MaxHP : 0.f);
		if (PlayerMP) PlayerMP->SetPercent(MaxMP > 0.f ? CurMP / MaxMP : 0.f);

		if (Text_PlayerHP) Text_PlayerHP->SetText(FText::AsNumber(FMath::RoundToInt(CurHP)));
		if (Text_PlayerMP) Text_PlayerMP->SetText(FText::AsNumber(FMath::RoundToInt(CurMP)));
	}

<<<<<<< Updated upstream
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
	// 기존의 bTeamUIInitialized 체크는 삭제하거나, 
	// 방의 최대 인원수(예: 4명)가 모두 찼을 때만 작동하도록 변경해야 합니다.
	// if (bTeamUIInitialized) return; 

	APlayerController* LocalPC = GetOwningPlayer();
	FString InstanceName = LocalPC ? LocalPC->GetName() : TEXT("UNKNOWN_CLIENT");

	// UE_LOG(LogTemp, Warning, TEXT("[%s] Attempting to bind %d PlayerStates."), *InstanceName, AllPlayerStates.Num());

	for (APlayerState* PS : AllPlayerStates)
	{
		AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS);
		if (!AFPS) continue;

		// ★ 핵심 수정: 이미 바인딩을 완료한 플레이어는 다시 처리하지 않음 (중복 바인딩 방지)
		if (BoundPlayerStates.Contains(AFPS))
		{
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("[%s] New Player Detected! Binding: %s (Team %d)"),
			*InstanceName, *AFPS->GetPlayerName(), AFPS->GetTeamID());

		// TeamPlayerStates Map 업데이트 (AddUnique 사용으로 중복 방지)
		TeamPlayerStates.FindOrAdd(AFPS->GetTeamID()).AddUnique(AFPS);

		// ==============
		// 1. 이름 설정
		// ==============
		FText PlayerNameText = FText::FromString(AFPS->GetPlayerName());
		if (AFPS->GetTeamID() == 0) // RED
		{
			if (AFPS->GetTeamIndex() == 1 && RedPlayerName01) RedPlayerName01->SetText(PlayerNameText);
			if (AFPS->GetTeamIndex() == 2 && RedPlayerName02) RedPlayerName02->SetText(PlayerNameText);
		}
		else // BLUE
		{
			if (AFPS->GetTeamIndex() == 1 && BluePlayerName01) BluePlayerName01->SetText(PlayerNameText);
			if (AFPS->GetTeamIndex() == 2 && BluePlayerName02) BluePlayerName02->SetText(PlayerNameText);
		}

		// ==============
		// 2. 델리게이트 바인딩 (로컬/스코어보드/팀스코어)
		// ==============

		// A. 로컬 전용 (HUD)
		if (AFPS->GetOwner() == LocalPC)
		{
			AFPS->OnHealthChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdateMyHealthBar);
			AFPS->OnManaChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdateMyManaBar);
			UpdateMyHealthBar(AFPS->GetCurrentHealth(), AFPS->GetMaxHealth(), AFPS);
			UpdateMyManaBar(AFPS->GetCurrentMana(), AFPS->GetMaxMana(), AFPS);
		}

		// B. 스코어보드 개별 바인딩
		AFPS->OnHealthChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdatePlayerHealthBar);
		AFPS->OnManaChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdatePlayerManaBar);
		AFPS->OnKillCountChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdatePlayerKillCount);
		AFPS->OnDeathCountChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdatePlayerDeathCount);

		// C. 팀 총합 스코어 바인딩
		AFPS->OnKillCountChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdateTeamKillDeathScore);
		AFPS->OnDeathCountChanged.AddUniqueDynamic(this, &UAFInGameWidget::UpdateTeamKillDeathScore);

		// 초기값 즉시 반영
		UpdatePlayerHealthBar(AFPS->GetCurrentHealth(), AFPS->GetMaxHealth(), AFPS);
		UpdatePlayerManaBar(AFPS->GetCurrentMana(), AFPS->GetMaxMana(), AFPS);
		UpdatePlayerKillCount(AFPS->GetKillCount(), AFPS);
		UpdatePlayerDeathCount(AFPS->GetDeathCount(), AFPS);

		// ★ 바인딩 완료 목록에 추가
		BoundPlayerStates.Add(AFPS);
	}

	// 팀 점수 최종 갱신
	UpdateTeamKillDeathScore(0, nullptr);

	// 최대 인원이 다 찼을 때만 초기화 완료 플래그 설정 (예: 4명 기준)
	if (BoundPlayerStates.Num() >= 4)
	{
		bTeamUIInitialized = true;
		UE_LOG(LogTemp, Warning, TEXT("[%s] ALL PLAYERS BOUND (%d/4)."), *InstanceName, BoundPlayerStates.Num());
	}
}

// 팀 스코어보드 핸들러
void UAFInGameWidget::UpdatePlayerHealthBar(float CurrentHealth, float MaxHealth, AAFPlayerState* TargetPS)
{
	if (!TargetPS || MaxHealth <= 0.f) return;
	float Percent = CurrentHealth / MaxHealth;

=======
	// B. 상단 팀 스코어보드 업데이트 (팀 ID와 인덱스로 위젯 검색)
>>>>>>> Stashed changes
	UProgressBar* TargetHPBar = nullptr;
	UProgressBar* TargetMPBar = nullptr;

	bool bIsRed = (TargetPS->GetTeamID() == 0);
	uint8 Idx = TargetPS->GetTeamIndex();

	if (bIsRed) {
		TargetHPBar = (Idx == 1) ? RedPlayer1HP : RedPlayer2HP;
		TargetMPBar = (Idx == 1) ? RedPlayer1MP : RedPlayer2MP;
	}
	else {
		TargetHPBar = (Idx == 1) ? BluePlayer1HP : BluePlayer2HP;
		TargetMPBar = (Idx == 1) ? BluePlayer1MP : BluePlayer2MP;
	}

	if (TargetHPBar) TargetHPBar->SetPercent(MaxHP > 0.f ? CurHP / MaxHP : 0.f);
	if (TargetMPBar) TargetMPBar->SetPercent(MaxMP > 0.f ? CurMP / MaxMP : 0.f);
}

void UAFInGameWidget::HandleScoreChanged(int32 Kills, int32 Deaths, AAFPlayerState* TargetPS)
{
	if (!IsValid(TargetPS)) return;

	UTextBlock* KillText = nullptr;
	UTextBlock* DeathText = nullptr;

	bool bIsRed = (TargetPS->GetTeamID() == 0);
	uint8 Idx = TargetPS->GetTeamIndex();

	if (bIsRed) {
		KillText = (Idx == 1) ? RedPlayer1Kill : RedPlayer2Kill;
		DeathText = (Idx == 1) ? RedPlayer1Death : RedPlayer2Death;
	}
	else {
		KillText = (Idx == 1) ? BluePlayer1Kill : BluePlayer2Kill;
		DeathText = (Idx == 1) ? BluePlayer1Death : BluePlayer2Death;
	}

	if (KillText) KillText->SetText(FText::AsNumber(Kills));
	if (DeathText) DeathText->SetText(FText::AsNumber(Deaths));

	// 개별 점수가 바뀔 때 팀 전체 점수판도 갱신
	UpdateTeamTotalScore();
}

void UAFInGameWidget::HandlePlayerInfoChanged(AAFPlayerState* PS)
{
	if (!IsValid(PS)) return;
	
	UTextBlock* NameWidget = nullptr;
	bool bIsRed = (PS->GetTeamID() == 0);
	uint8 Idx = PS->GetTeamIndex();

	if (bIsRed) {
		NameWidget = (Idx == 1) ? RedPlayerName01 : RedPlayerName02;
	}
	else {
		NameWidget = (Idx == 1) ? BluePlayerName01 : BluePlayerName02;
	}

	if (NameWidget)
	{
		FText DisplayName = FText::FromString(PS->GetPlayerName());

		// 사망 상태라면 이름 옆에 표시
		if (PS->IsDead())
		{
			DisplayName = FText::Format(FText::FromString(TEXT("{0} (Dead)")), DisplayName);
			NameWidget->SetColorAndOpacity(FLinearColor::Gray);
		}
		else
		{
			NameWidget->SetColorAndOpacity(FLinearColor::White);
		}

		NameWidget->SetText(DisplayName);
	}
}

<<<<<<< Updated upstream
// 팀 총합 스코어 갱신 핸들러
void UAFInGameWidget::UpdateTeamKillDeathScore(int32 NewValue, AAFPlayerState* TargetPS)
{

	RedTotalKills = 0;
	RedTotalDeaths = 0;
	BlueTotalKills = 0;
	BlueTotalDeaths = 0;

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
=======
>>>>>>> Stashed changes
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

	if (NewTime <= 0)
	{
		ShowGameResult();
	}
}

void UAFInGameWidget::UpdateTeamTotalScore()
{
<<<<<<< Updated upstream
	APlayerController* PC = GetOwningPlayer();
	AAFPlayerController* AFPC = Cast<AAFPlayerController>(PC);
	if (!AFPC) return;
	AAFPlayerState* MyPS = PC ? PC->GetPlayerState<AAFPlayerState>() : nullptr;
	if (!MyPS) return;


	// 내 팀 확인 및 승패 판정
	uint8 MyTeam = MyPS->GetTeamID();
	bool bIWin = false;
	bool bIsDraw = (BlueTotalKills == RedTotalKills);
=======
	AAFGameState* GS = GetWorld()->GetGameState<AAFGameState>();
	if (!GS) return;

	if (RedKillScore) RedKillScore->SetText(FText::AsNumber(GS->TeamRedKillScore));
	if (RedDeathScore) RedDeathScore->SetText(FText::AsNumber(GS->TeamRedDeathScore));
	if (BlueKillScore) BlueKillScore->SetText(FText::AsNumber(GS->TeamBlueKillScore));
	if (BlueDeathScore) BlueDeathScore->SetText(FText::AsNumber(GS->TeamBlueDeathScore));
}
#pragma endregion

// =============================
// 2. 내부 로직 및 바인딩
// =============================
#pragma region Binding

void UAFInGameWidget::HandlePlayerArrayChanged()
{
	AAFGameState* GS = GetWorld()->GetGameState<AAFGameState>();
	if (!GS) return;
>>>>>>> Stashed changes

	for (APlayerState* PS : GS->PlayerArray)
	{
<<<<<<< Updated upstream
		if (RedTotalKills > BlueTotalKills) bIWin = (MyTeam == 0);
		else bIWin = (MyTeam == 1);
	}

	// 3. 위젯 생성 및 출력
	TSubclassOf<UUserWidget> ResultClass = nullptr;

	if (bIsDraw)
	{
		ResultClass = DrawWidgetClass;
	}
	else
	{
		ResultClass = bIWin ? VictoryWidgetClass : LoseWidgetClass;
	}
	
	if (ResultClass)
	{
		UUserWidget* ResultWidget = CreateWidget<UUserWidget>(GetWorld(), ResultClass);
		if (ResultWidget)
=======
		if (AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS))
>>>>>>> Stashed changes
		{
			BindToPlayerState(AFPS);
		}
	}
}

void UAFInGameWidget::BindToPlayerState(AAFPlayerState* PS)
{
	if (!PS || BoundPlayerStates.Contains(PS)) return;

	UAFAttributeComponent* AttribComp = PS->FindComponentByClass<UAFAttributeComponent>();

	// 통합 델리게이트 바인딩
	PS->OnAttributeChanged.AddUniqueDynamic(this, &UAFInGameWidget::HandleAttributeChanged);
	PS->OnScoreChanged.AddUniqueDynamic(this, &UAFInGameWidget::HandleScoreChanged);
	PS->OnPlayerInfoChanged.AddUniqueDynamic(this, &UAFInGameWidget::HandlePlayerInfoChanged);

	// 초기값 강제 갱신
	if (AttribComp)
	{
		HandleAttributeChanged(
			AttribComp->GetHealth(),
			AttribComp->GetMaxHealth(),
			AttribComp->GetMana(),
			AttribComp->GetMaxMana(),
			PS
		);
	}


	HandleScoreChanged(PS->GetKillCount(), PS->GetDeathCount(), PS);
	HandlePlayerInfoChanged(PS);

	BoundPlayerStates.Add(PS);

	UE_LOG(LogTemp, Log, TEXT("UI Bound to Player: %s"), *PS->GetPlayerName());
}

void UAFInGameWidget::ShowGameResult()
{
	AAFGameState* GS = GetWorld()->GetGameState<AAFGameState>();
	AAFPlayerState* MyPS = GetOwningPlayerState<AAFPlayerState>();
	if (!GS || !MyPS) return;

	int32 RedKills = GS->TeamRedKillScore;
	int32 BlueKills = GS->TeamBlueKillScore;

	TSubclassOf<UUserWidget> ResultClass = DrawWidgetClass;

	if (RedKills != BlueKills)
	{
		bool bRedWin = RedKills > BlueKills;
		bool bIsMyTeamRed = (MyPS->GetTeamID() == 0);

		ResultClass = (bRedWin == bIsMyTeamRed) ? VictoryWidgetClass : LoseWidgetClass;
	}

	if (ResultClass)
	{
		UUserWidget* ResultWidget = CreateWidget<UUserWidget>(GetWorld(), ResultClass);
		if (ResultWidget) ResultWidget->AddToViewport();
	}
}
#pragma endregion














