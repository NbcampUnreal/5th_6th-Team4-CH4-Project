// AFPlayerState.cpp


#include "Player/AFPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Player/AFPlayerController.h"
#include "Net/UnrealNetwork.h"

AAFPlayerState::AAFPlayerState()
{
	MaxHealth = 100.0f;
	MaxMana = 100.0f;
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	KillCount = 0;
	DeathCount = 0;
	TeamID = 0; // Default RED
	TeamIndex = 1; // Default Index 1
}

void AAFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAFPlayerState, CurrentHealth);
	DOREPLIFETIME(AAFPlayerState, MaxHealth);
	DOREPLIFETIME(AAFPlayerState, CurrentMana);
	DOREPLIFETIME(AAFPlayerState, MaxMana);
	DOREPLIFETIME(AAFPlayerState, KillCount);
	DOREPLIFETIME(AAFPlayerState, DeathCount);
	DOREPLIFETIME(AAFPlayerState, TeamID);
	DOREPLIFETIME(AAFPlayerState, TeamIndex);
}

// =========================
// OnRep 함수 구현
// =========================
void AAFPlayerState::OnRep_CurrentHealth()
{
			OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, this);
}

void AAFPlayerState::OnRep_CurrentMana()
{
	OnManaChanged.Broadcast(CurrentMana, MaxMana, this);
}

void AAFPlayerState::OnRep_KillCount()
{
	OnKillCountChanged.Broadcast(KillCount, this);
}

void AAFPlayerState::OnRep_DeathCount()
{
	OnDeathCountChanged.Broadcast(DeathCount, this);
}


// =========================
// Setter 구현
// =========================

void AAFPlayerState::SetHealth(float NewHealth, float NewMaxHealth)
{
	if (!HasAuthority()) return;

	// 값이 실제로 변경되었는지 확인
	bool bHealthChanged = (CurrentHealth != NewHealth || MaxHealth != NewMaxHealth);

	CurrentHealth = FMath::Clamp(NewHealth, 0.f, NewMaxHealth);
	MaxHealth = NewMaxHealth;

	// 서버에서 UI를 갱신하기 위해 수동으로 OnRep 함수 호출
	if (bHealthChanged)
	{
		OnRep_CurrentHealth();
		// 클라이언트들은 엔진의 복제 시스템에 의해 OnRep_CurrentHealth가 호출됩니다.
	}
}

void AAFPlayerState::SetMana(float NewMana, float NewMaxMana)
{
	if (!HasAuthority()) return;

	bool bManaChanged = (CurrentMana != NewMana || MaxMana != NewMaxMana);

	CurrentMana = FMath::Clamp(NewMana, 0.f, NewMaxMana);
	MaxMana = NewMaxMana;

	if (bManaChanged)
	{
		OnRep_CurrentMana();
	}
}

void AAFPlayerState::IncrementKillCount()
{
	if (!HasAuthority()) return;

	KillCount++;
	// 서버에서 UI 갱신을 위해 수동 호출
	OnRep_KillCount();
}

void AAFPlayerState::IncrementDeathCount()
{
	if (!HasAuthority()) return;

	DeathCount++;
	// 서버에서 UI 갱신을 위해 수동 호출
	OnRep_DeathCount();
}

void AAFPlayerState::SetTeamInfo(uint8 NewTeamID, uint8 NewTeamIndex)
{
	if (!HasAuthority()) return;

	TeamID = NewTeamID;
	TeamIndex = NewTeamIndex;

	// TeamID와 TeamIndex는 복제되어 클라이언트에게 전달됩니다.
}




void AAFPlayerState::AddMana(float Amount)
{
	if (!HasAuthority()) return;

	SetMana(CurrentMana + Amount, MaxMana);
}

bool AAFPlayerState::ConsumeMana(float Amount)
{
	if (!HasAuthority()) return false;

	if (CurrentMana < Amount)
	{
		return false;
	}

	SetMana(CurrentMana - Amount, MaxMana);
	return true;
}










