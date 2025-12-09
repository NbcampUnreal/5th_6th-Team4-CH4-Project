// AFGameState.cpp


#include "Game/AFGameState.h"
#include "Net/UnrealNetwork.h"

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