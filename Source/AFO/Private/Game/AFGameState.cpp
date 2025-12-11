// AFGameState.cpp


#include "Game/AFGameState.h"
#include "Net/UnrealNetwork.h"

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

	// 남은 시간 변수 복제 및 RepNotify 설정
	DOREPLIFETIME(ThisClass, RemainingTimeSeconds);

	// Phase 변수 복제
	DOREPLIFETIME(ThisClass, CurrentGamePhase);
}

// RepNotify 함수 구현
void AAFGameState::OnRep_RemainingTime()
{
	// 인게임 UI에서 이 함수를 호출하기
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
		// 시간이 0이 되면 타이머 정지 (나중에 게임 종료 로직 추가)
		GetWorldTimerManager().ClearTimer(GameTimerHandle);
	}
}

void AAFGameState::OnRep_RemainingTime()
{
	// 델리게이트 방송 -> 이걸 구독하고 있는 위젯(HUD)이 반응함
	OnTimerChanged.Broadcast(RemainingTimeSeconds);
}