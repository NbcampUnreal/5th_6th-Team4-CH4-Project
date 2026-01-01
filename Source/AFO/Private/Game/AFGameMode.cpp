#include "Game/AFGameMode.h"
#include "Game/AFGameState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "AFO/Public/Player/AFPlayerState.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Player/AFPlayerController.h"
#include "Game/AFGameInstance.h"
#include "Character/AFPlayerCharacter.h"

AAFGameMode::AAFGameMode()
{
	GameStateClass = AAFGameState::StaticClass();
	RoundDuration = 180;
}

AAFGameState* AAFGameMode::GetAFGameState() const
{
	return GetGameState<AAFGameState>();
}

void AAFGameMode::BeginPlay()
{
	Super::BeginPlay();

	AAFGameState* GS = GetAFGameState();
	if (!GS)
	{
		UE_LOG(LogTemp, Error, TEXT("AFGameMode::BeginPlay - GameState is null"));
		return;
	}

	GS->SetGamePhase(EAFGamePhase::EAF_InGame);
	GS->TeamRedKillScore = 0;
	GS->TeamBlueKillScore = 0;



	// 1. 모든 플레이어의 입력을 처음에 막음
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			PC->SetInputMode(FInputModeGameOnly());
			// 캐릭터 조작 금지 (Move 등을 못하게 함)
			if (APawn* P = PC->GetPawn())
			{
				P->DisableInput(PC);
			}
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandle_GameStart, this, &AAFGameMode::StartRound, (float)StartDelayTime, false);

	UE_LOG(LogTemp, Log, TEXT("Game will start in 5 seconds..."));


}

void AAFGameMode::StartRound()
{
	AAFGameState* GS = GetAFGameState();
	if (!GS)
	{
		UE_LOG(LogTemp, Error, TEXT("AFGameMode::StartRound - GameState is null"));
		return;
	}

	// 캐릭터 움직이기 가능
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (APawn* P = PC->GetPawn())
			{
				P->EnableInput(PC);
			}
		}
	}




	GS->SetRemainingTime(RoundDuration);
	GS->StartGameTimer();

	UE_LOG(LogTemp, Warning, TEXT("Round Start, Duration: %d"), RoundDuration);
}

void AAFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!ensureMsgf(NewPlayer, TEXT("PostLogin: NewPlayer is nullptr!")))
	{
		return;
	}

	uint8 AssignedTeam = (PlayerTeams.Num() % 2 == 0) ? 0 : 1; // 0: RED, 1: BLUE
	PlayerTeams.Add(NewPlayer, AssignedTeam);

	AAFPlayerState* PS = NewPlayer->GetPlayerState<AAFPlayerState>();
	if (PS)
	{
		int32 TeamMemberIndex = 0;
		for (const auto& Pair : PlayerTeams)
		{
			if (Pair.Value == AssignedTeam)
			{
				TeamMemberIndex++;
			}
		}

		PS->SetTeamInfo(AssignedTeam, static_cast<uint8>(TeamMemberIndex));

		UE_LOG(LogTemp, Log, TEXT("[AFO] Player Joined: %s | Team: %s | TeamIdx: %d"),
			*NewPlayer->GetName(),
			AssignedTeam == 0 ? TEXT("RED") : TEXT("BLUE"),
			TeamMemberIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: Failed to cast to AAFPlayerState!"), *FString(__FUNCTION__));
	}
}

void AAFGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		PlayerTeams.Remove(PC);

		TWeakObjectPtr<AController> Weak = PC;
		if (FTimerHandle* H = RespawnTimers.Find(Weak))
		{
			GetWorldTimerManager().ClearTimer(*H);
			RespawnTimers.Remove(Weak);
		}

		UE_LOG(LogTemp, Warning, TEXT("Player Left Game: %s"), *PC->GetName());
	}
}

void AAFGameMode::ReportKill(AController* KillerController)
{
	AAFGameState* GS = GetAFGameState();
	if (!GS || !KillerController)
	{
		return;
	}

	AAFPlayerState* KillerPS = KillerController->GetPlayerState<AAFPlayerState>();
	if (!KillerPS)
	{
		return;
	}

	const uint8 KillerTeam = KillerPS->GetTeamID(); // 0: Red, 1: Blue

	if (KillerTeam == 0)
	{
		GS->TeamRedKillScore++;
	}
	else
	{
		GS->TeamBlueKillScore++;
	}

	UE_LOG(LogTemp, Warning, TEXT("Kill → Team:%s | Red:%d / Blue:%d"),
		(KillerTeam == 0) ? TEXT("RED") : TEXT("BLUE"),
		GS->TeamRedKillScore,
		GS->TeamBlueKillScore);
}

// 임시로 기존 함수 비활성화

AActor* AAFGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AAFPlayerState* PS = Player ? Player->GetPlayerState<AAFPlayerState>() : nullptr;
	if (!PS)
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	const FName WantedTag = (PS->GetTeamID() == 0) ? FName(TEXT("Red")) : FName(TEXT("Blue"));

	TArray<AActor*> Starts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Starts);

	TArray<APlayerStart*> Candidates;
	for (AActor* A : Starts)
	{
		APlayerStart* S = Cast<APlayerStart>(A);
		if (S && S->PlayerStartTag == WantedTag)
		{
			Candidates.Add(S);
		}
	}

	if (Candidates.Num() > 0)
	{
		return Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void AAFGameMode::HandlePlayerDeath(AController* VictimController, AController* KillerController)
{
	if (!HasAuthority() || !VictimController) return;

	AAFPlayerState* VictimPS = VictimController->GetPlayerState<AAFPlayerState>();
	AAFGameState* GS = GetAFGameState();
	if (!VictimPS || !GS) return;

<<<<<<< Updated upstream
	if (VictimPS->IsDead())
	{
		return;
	}

	VictimPS->IncrementDeathCount();
	VictimPS->SetDead(true);
	AAFPlayerState* KillerPS = KillerController->GetPlayerState<AAFPlayerState>();

	if (KillerController && KillerController != VictimController)
	{
		{
			KillerPS->IncrementKillCount();
		}

		ReportKill(KillerController);
=======
	// 1. 피해자 처리
	VictimPS->AddDeath(); 
	GS->AddTeamScore(VictimPS->GetTeamID(), false); // 팀 데스 추가

	// 2. 킬러 처리
	AAFPlayerState* KillerPS = KillerController ? KillerController->GetPlayerState<AAFPlayerState>() : nullptr;
	if (KillerPS && KillerPS != VictimPS)
	{
		KillerPS->AddKill(); // 개인 킬++
		GS->AddTeamScore(KillerPS->GetTeamID(), true); // 팀 킬 추가
>>>>>>> Stashed changes
	}
	

	if (APawn* Pawn = VictimController->GetPawn())
	{
		Pawn->DetachFromControllerPendingDestroy();
		Pawn->SetLifeSpan(5.0f);
		//Pawn->Destroy();
	}

	// kill log
	if (VictimPS)
	{
		// 1. 이름 및 색상 정보 준비
		FString KillerName = KillerPS ? KillerPS->GetPlayerName() : TEXT("Environment");
		FString VictimName = VictimPS->GetPlayerName();

		// 팀 ID에 따른 색상 (0: Red, 1: Blue)
		FLinearColor KillerColor = (KillerPS && KillerPS->GetTeamID() == 0) ? FLinearColor::Red : FLinearColor::Blue;
		FLinearColor VictimColor = (VictimPS->GetTeamID() == 0) ? FLinearColor::Red : FLinearColor::Blue;

		// 자살(Fall Death 등)일 경우 Killer를 Environment나 본인 이름으로 처리
		if (KillerController == VictimController || KillerController == nullptr)
		{
			KillerName = TEXT("System");
			KillerColor = FLinearColor::Gray;
		}

		// 2. 모든 접속 중인 플레이어에게 킬 로그 브로드캐스트
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AAFPlayerController* PC = Cast<AAFPlayerController>(It->Get()))
			{
				// 각 클라이언트의 RPC 함수 호출!
				PC->Client_ShowKillLog(KillerName, KillerColor, VictimName, VictimColor);
			}
		}
	}




	// 리스폰 위젯 띄우기
	if (AAFPlayerController* PC = Cast<AAFPlayerController>(VictimController))
	{
		PC->Client_ClearRespawnWidget();
		PC->Client_ShowRespawnWidget(RespawnDelay);
	}

	// 10초 후 리스폰
	TWeakObjectPtr<AController> WeakVictim = VictimController;

	FTimerHandle& Handle = RespawnTimers.FindOrAdd(WeakVictim);

	GetWorldTimerManager().SetTimer(
		Handle,
		FTimerDelegate::CreateWeakLambda(this, [this, WeakVictim]()
			{
				if (!WeakVictim.IsValid()) return;

				AController* VC = WeakVictim.Get();

				if (AAFPlayerState* PS = VC->GetPlayerState<AAFPlayerState>())
				{
					PS->ResetForRespawn();
				}

				RestartPlayer(VC);

				RespawnTimers.Remove(WeakVictim); // 정리
			}),
		RespawnDelay,
		false
	);
}

void AAFGameMode::EndRound()
{
	AAFGameState* GS = GetAFGameState();
	if (!GS) return;

<<<<<<< Updated upstream
	if (GS->TeamRedKillScore > GS->TeamBlueKillScore)
	{
		UE_LOG(LogTemp, Warning, TEXT("RED TEAM WINS THE ROUND"));
	}
	else if (GS->TeamBlueKillScore > GS->TeamRedKillScore)
	{
		UE_LOG(LogTemp, Warning, TEXT("BLUE TEAM WINS THE ROUND"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DRAW ROUND"));
	}

=======
	// 1. 승자 판정 로직 (기존 유지)
	EAFTeamId WinnerTeam = EAFTeamId::None;
	if (GS->TeamRedKillScore > GS->TeamBlueKillScore) WinnerTeam = EAFTeamId::Red;
	else if (GS->TeamBlueKillScore > GS->TeamRedKillScore) WinnerTeam = EAFTeamId::Blue;

	GS->SetMatchResult(WinnerTeam);
>>>>>>> Stashed changes
	GS->SetGamePhase(EAFGamePhase::EAF_GameOver);

	// ==========================================================
	// [추가 포인트] 2. GameInstance에 최종 통계 저장
	// 레벨이 넘어가기(ServerTravel) 직전에 모든 PS의 데이터를 담습니다.
	// ==========================================================
	if (UAFGameInstance* GI = Cast<UAFGameInstance>(GetGameInstance()))
	{
		GI->ClearResults(); // 이전 판 데이터 삭제

		for (APlayerState* PS : GS->PlayerArray)
		{
			if (AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS))
			{
				FAFGameResultData Result;
				Result.PlayerName = AFPS->GetPlayerName();
				Result.Kills = AFPS->GetKillCount();
				Result.Deaths = AFPS->GetDeathCount();
				// 아래 데이터는 PlayerState에 추가할 예정인 변수들
				Result.DamageDealt = AFPS->GetTotalDamageDealt();
				Result.DamageTaken = AFPS->GetTotalDamageTaken();
				Result.Healing = AFPS->GetTotalHealingDone();

				GI->FinalMatchResults.Add(Result);
			}
		}
	}

	// 3. 레벨 이동 (5초 대기)
	if (UWorld* World = GetWorld())
	{
<<<<<<< Updated upstream
		World->ServerTravel(TEXT("TitleMenu"));
=======
		FTimerHandle TravelHandle;
		World->GetTimerManager().SetTimer(TravelHandle, FTimerDelegate::CreateLambda([World]()
			{
				// SeamlessTravel을 사용하여 결과창 레벨로 이동
				World->ServerTravel(TEXT("/Game/Maps/Lobby/ResultLevel?Level=Result"));
			}), 5.0f, false);
>>>>>>> Stashed changes
	}
}

UClass* AAFGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	AAFPlayerState* PS = InController ? InController->GetPlayerState<AAFPlayerState>() : nullptr;
	if (PS)
	{
		const uint8 CharId = PS->GetSelectedCharacterId();
		if (PS->HasSelectedCharacter()
			&& CharacterPawnClasses.IsValidIndex(CharId)
			&& CharacterPawnClasses[CharId])
		{
			return CharacterPawnClasses[CharId];
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AAFGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	if (!C) return;

	if (APawn* OldPawn = C->GetPawn())
	{
		OldPawn->Destroy();
	}

	RestartPlayer(C); // 여기서 위 GetDefaultPawnClassForController가 적용됨
}

void AAFGameMode::RestartPlayer(AController* NewPlayer)
{
	if (!NewPlayer) return;

	AAFPlayerState* PS = NewPlayer->GetPlayerState<AAFPlayerState>();
	// 1. 이름 검사: Unknown이거나 비어있으면 Mage 등으로 강제 배정 (테스트용)
	FString SelectedName = PS ? PS->GetSelectedCharacterKey() : TEXT("Mage");

	if (SelectedName.IsEmpty() || SelectedName.Equals(TEXT("Unknown")))
	{
		UE_LOG(LogTemp, Error, TEXT("@@@ [GameMode] Invalid Character Name! Forcing 'Mage' for testing."));
		SelectedName = TEXT("Mage");
	}

	Super::RestartPlayer(NewPlayer); // 여기서 Pawn이 스폰됨

	AAFPlayerCharacter* AFChar = Cast<AAFPlayerCharacter>(NewPlayer->GetPawn());
	if (AFChar)
	{
		// 2. 스폰 직후 데이터 주입
		AFChar->InitializeCharacterData(SelectedName);
		UE_LOG(LogTemp, Warning, TEXT("@@@ [GameMode] Initialized %s for %s"), *SelectedName, *NewPlayer->GetName());
	}
}

//// 캐릭터 선택 화면 구현 전 임시 캐릭터 설정 함수
//UClass* AAFGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
//{
//	// 접속한 순서에 따라 클래스 배정
//	PlayerCount++;
//
//	UE_LOG(LogTemp, Warning, TEXT("Player Joined! Current Count: %d"), PlayerCount);
//
//	if (PlayerCount == 1 && FirstCharacterClass)
//	{
//		return FirstCharacterClass;
//	}
//	else if (PlayerCount == 2 && SecondCharacterClass)
//	{
//		return SecondCharacterClass;
//	}
//	else if (PlayerCount == 3 && ThirdCharacterClass)
//	{
//		return ThirdCharacterClass;
//	}
//	else if (PlayerCount == 4 && FourthCharacterClass)
//	{
//		return FourthCharacterClass;
//	}
//
//	// 3번째 이후거나 설정이 안 되어있으면 기본 Pawn 클래스 사용
//	return Super::GetDefaultPawnClassForController_Implementation(InController);
//}
//
//
//AActor* AAFGameMode::ChoosePlayerStart_Implementation(AController* Player)
//{
//	// 1. 서버 권한 및 유효성 검사
//	if (!Player) return nullptr;
//
//	// 2. AFO 플레이어 상태 정보 가져오기
//	AAFPlayerState* PS = Player->GetPlayerState<AAFPlayerState>();
//	if (PS)
//	{
//		// 현재 로그에 찍히고 있는 정보를 기반으로 가져옴
//		int32 TargetTeam = PS->GetTeamID();     // 0 또는 1
//		int32 TargetIndex = PS->GetTeamIndex(); // 0 또는 1
//
//		// 태그 생성: "0_0", "0_1" 등
//		FString TagStr = FString::Printf(TEXT("%d_%d"), TargetTeam, TargetIndex);
//		FName TargetTag = FName(*TagStr);
//
//		UE_LOG(LogTemp, Warning, TEXT("[AFO] Player Connected! Team: %d, Idx: %d. Finding StartTag: %s"), TargetTeam, TargetIndex, *TagStr);
//
//		// 3. 해당 태그를 가진 PlayerStart 찾기
//		TArray<AActor*> FoundActors;
//		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), TargetTag, FoundActors);
//
//		if (FoundActors.Num() > 0)
//		{
//			return FoundActors[0];
//		}
//	}
//
//	// 정보를 못 찾을 경우 기본 PlayerStart 반환
//	UE_LOG(LogTemp, Error, TEXT("[AFO] Failed to find specific PlayerStart. Using default."));
//	return Super::ChoosePlayerStart_Implementation(Player);
//}