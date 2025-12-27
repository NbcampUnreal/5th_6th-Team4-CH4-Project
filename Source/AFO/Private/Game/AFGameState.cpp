// AFGameState.cpp


#include "Game/AFGameState.h"
#include "Net/UnrealNetwork.h"
#include "Game/AFGameMode.h"
#include"AFO/Public/Player/AFPlayerState.h"

AAFGameState::AAFGameState()
{
	RemainingTimeSeconds = 300.f;
}

void AAFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Score 변수 복제
	DOREPLIFETIME(ThisClass, TeamRedKillScore);
	DOREPLIFETIME(ThisClass, TeamBlueKillScore);
	DOREPLIFETIME(ThisClass, TeamRedDeathScore);
	DOREPLIFETIME(ThisClass, TeamBlueDeathScore);

	// 남은 시간 변수 복제 및 RepNotify 설정
	DOREPLIFETIME(ThisClass, RemainingTimeSeconds);

	// Phase 변수 복제
	DOREPLIFETIME(ThisClass, CurrentGamePhase);
	DOREPLIFETIME(AAFGameState, TeamPlayerStatesReplicated);
}

// 서버 권한 : 시간 설정
void AAFGameState::SetRemainingTime(int32 NewTime)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RemainingTimeSeconds = NewTime;
		OnRep_RemainingTime();
	}
}

// 서버 권한 : 게임 단계 설정
void AAFGameState::SetGamePhase(EAFGamePhase NewPhase)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentGamePhase = NewPhase;
		// 재현님 이 변수 변경을 토대로 화면 전환 해주시면 됩니다
	}
}


void AAFGameState::StartGameTimer()
{
	// 서버에서만 실행
	if (GetLocalRole() == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(
			GameTimerHandle,
			this,
			&AAFGameState::UpdateTimer,
			1.0f, // 1초 간격
			true  // 반복
		);
	}
}

void AAFGameState::UpdateTimer()
{
	if (RemainingTimeSeconds > 0)
	{
		RemainingTimeSeconds--;

		// 서버는 OnRep이 자동 호출되지 않으므로 직접 호출하여 델리게이트를 발생시킴
		OnRep_RemainingTime();
	}
	else
	{
		// 시간이 0이 되면 타이머 정지 후 게임 종료 (타이틀로 이동)
		GetWorldTimerManager().ClearTimer(GameTimerHandle);

		if (GetLocalRole() == ROLE_Authority)
		{
			if (AAFGameMode* GM = GetWorld()->GetAuthGameMode<AAFGameMode>())
			{
				// GM->EndRound();
			}
		}
	}
}


void AAFGameState::OnRep_RemainingTime()
{
	// 델리게이트 방송 -> 이걸 구독하고 있는 위젯(HUD)이 반응함
	OnTimerChanged.Broadcast(RemainingTimeSeconds);
}


void AAFGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (HasAuthority()) // 서버에서만 실행
	{
		// PlayerArray가 업데이트된 후 RepNotify 변수를 갱신하고 강제 복제
		AAFPlayerState* AFPS = Cast<AAFPlayerState>(PlayerState);
		if (AFPS)
		{
			// PlayerArray를 직접 사용해도 되지만, 복제 변수에 추가하여 RepNotify를 유발
			TeamPlayerStatesReplicated.Add(AFPS);
			OnRep_TeamPlayerArray(); // 서버에서 수동으로 RepNotify 실행하여 모든 클라이언트에게 알림
		}
	}
}

void AAFGameState::OnRep_TeamPlayerArray()
{
	// 모든 클라이언트에서 실행됨
	OnPlayerArrayChanged.Broadcast();
}

void AAFGameState::AddTeamScore(uint8 TeamID, bool bIsKill)
{
	if (!HasAuthority()) return;

	FString ScoreType = bIsKill ? TEXT("KILL") : TEXT("DEATH");
	FString TeamName = (TeamID == 0) ? TEXT("RED") : TEXT("BLUE");

	if (TeamID == 0)
	{
		if (bIsKill) TeamRedKillScore++;
		else TeamRedDeathScore++;
	}
	else
	{
		if (bIsKill) TeamBlueKillScore++;
		else TeamBlueDeathScore++;
	}

	// ★ 서버 로그: 점수가 실제로 몇 점으로 변했는지 출력
	UE_LOG(LogTemp, Warning, TEXT("[SERVER] TeamScore Updated: Team=%s, Type=%s, CurrentTotalKill(R:%d, B:%d)"),
		*TeamName, *ScoreType, TeamRedKillScore, TeamBlueKillScore);

	OnRep_TeamScore();
}

void AAFGameState::OnRep_TeamScore()
{
	// 게임 스테이트에 선언해둔 기존 델리게이트나 새로운 델리게이트를 통해 UI에 알림
	// 여기서는 간단하게 기존 OnPlayerArrayChanged를 활용하거나 새 델리게이트를 만드세요.
	OnPlayerArrayChanged.Broadcast();
}