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

	// Score ���� ����
	DOREPLIFETIME(ThisClass, TeamRedKillScore);
	DOREPLIFETIME(ThisClass, TeamBlueKillScore);

	// ���� �ð� ���� ���� �� RepNotify ����
	DOREPLIFETIME(ThisClass, RemainingTimeSeconds);

	// Phase ���� ����
	DOREPLIFETIME(ThisClass, CurrentGamePhase);
}

// ���� ���� : �ð� ����
void AAFGameState::SetRemainingTime(int32 NewTime)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RemainingTimeSeconds = NewTime;
		OnRep_RemainingTime();
	}
}

// ���� ���� : ���� �ܰ� ����
void AAFGameState::SetGamePhase(EAFGamePhase NewPhase)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentGamePhase = NewPhase;
		// ������ �� ���� ������ ���� ȭ�� ��ȯ ���ֽø� �˴ϴ�
	}
}


void AAFGameState::StartGameTimer()
{
	// ���������� ����
	if (GetLocalRole() == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(
			GameTimerHandle,
			this,
			&AAFGameState::UpdateTimer,
			1.0f, // 1�� ����
			true  // �ݺ�
		);
	}
}

void AAFGameState::UpdateTimer()
{
	if (RemainingTimeSeconds > 0)
	{
		RemainingTimeSeconds--;

		// ������ OnRep�� �ڵ� ȣ����� �����Ƿ� ���� ȣ���Ͽ� ��������Ʈ�� �߻���Ŵ
		OnRep_RemainingTime();
	}
	else
	{
		// �ð��� 0�� �Ǹ� Ÿ�̸� ���� (���߿� ���� ���� ���� �߰�)
		GetWorldTimerManager().ClearTimer(GameTimerHandle);
	}
}

void AAFGameState::OnRep_RemainingTime()
{
	// ��������Ʈ ��� -> �̰� �����ϰ� �ִ� ����(HUD)�� ������
	OnTimerChanged.Broadcast(RemainingTimeSeconds);
}