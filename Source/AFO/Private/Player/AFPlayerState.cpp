// AFPlayerState.cpp


#include "Player/AFPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Player/AFPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Game/AFLobbyGameState.h"
#include "Components/AFAttributeComponent.h"

// ===========================================
// 0. 초기화 및 동기화
// ===========================================
#pragma region InitializeSetting
AAFPlayerState::AAFPlayerState()
{
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	KillCount = 0;
	DeathCount = 0;
	TeamID = 0; // Default RED
	TeamIndex = 1; // Default Index 1
	bReady = false;
	SelectedCharacterId = 255;
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
	DOREPLIFETIME(AAFPlayerState, SelectedCharacterId);
<<<<<<< Updated upstream
=======
	DOREPLIFETIME(AAFPlayerState, SelectedCharacterName);
>>>>>>> Stashed changes
	DOREPLIFETIME(AAFPlayerState, bReady);
}

void AAFPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (AAFPlayerState* NewPS = Cast<AAFPlayerState>(PlayerState))
	{
		NewPS->TeamID = TeamID;
		NewPS->TeamIndex = TeamIndex;
		NewPS->SelectedCharacterId = SelectedCharacterId;
		NewPS->bReady = bReady;
<<<<<<< Updated upstream
		UE_LOG(LogTemp, Warning, TEXT("CopyProperties: OldTeam=%d -> NewPS Team Set!"), TeamID);
=======
		NewPS->KillCount = KillCount;
		NewPS->DeathCount = DeathCount;
		NewPS->SelectedCharacterKey = this->SelectedCharacterKey;
>>>>>>> Stashed changes
	}
}

void AAFPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	if (AAFPlayerState* OldPS = Cast<AAFPlayerState>(PlayerState))
	{
		TeamID = OldPS->TeamID;
		TeamIndex = OldPS->TeamIndex;
		SelectedCharacterId = OldPS->SelectedCharacterId;
		SelectedCharacterName = OldPS->SelectedCharacterName;
		bReady = OldPS->bReady;
	}
}


#pragma endregion

// ===========================================
// 1. 복제변수 및 OnRep함수
// ===========================================
#pragma region ReplicatedVariable

void AAFPlayerState::OnRep_Attributes()// HP, MP 변화 시 호출
{
	OnAttributeChanged.Broadcast(CurrentHealth, MaxHealth, CurrentMana, MaxMana, this);
}
void AAFPlayerState::OnRep_PlayerScore()      // Kill, Death 변화 시 호출
{
	OnScoreChanged.Broadcast(KillCount, DeathCount, this);
}
void AAFPlayerState::OnRep_PlayerInfo() // 팀, 캐릭터 정보 변화 시 호출
{
<<<<<<< Updated upstream
	OnKillCountChanged.Broadcast(KillCount, this);
}

void AAFPlayerState::OnRep_DeathCount()
{
	OnDeathCountChanged.Broadcast(DeathCount, this);
}

void AAFPlayerState::OnRep_IsDead()
{
	// 여기다가 UI 갱신
}

void AAFPlayerState::OnRep_SelectedCharacter() 
{
	/* UI 갱신 가능 */ 
}
void AAFPlayerState::OnRep_Ready()
{

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

void AAFPlayerState::OnRep_TeamInfo()
{
	OnTeamInfoChanged.Broadcast(this);
=======
	OnPlayerInfoChanged.Broadcast(this);
>>>>>>> Stashed changes

	// 로비 게임스테이트가 있다면 인원수 갱신 요청 (기존 로직 유지)
	if (UWorld* World = GetWorld())
	{
		if (AAFLobbyGameState* LGS = World->GetGameState<AAFLobbyGameState>())
		{
			LGS->OnCountsChanged.Broadcast();
		}
	}
}
void AAFPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	OnRep_PlayerInfo();
}

#pragma endregion

// =========================
// 2. Setter 
// =========================
#pragma region Setter

void AAFPlayerState::SetHealth(float NewHealth, float NewMaxHealth)
{
	if (!HasAuthority()) return;
	CurrentHealth = FMath::Clamp(NewHealth, 0.f, NewMaxHealth);
	MaxHealth = NewMaxHealth;
	OnRep_Attributes();
}

void AAFPlayerState::SetMana(float NewMana, float NewMaxMana)
{
	if (!HasAuthority()) return;
	CurrentMana = FMath::Clamp(NewMana, 0.f, NewMaxMana);
	MaxMana = NewMaxMana;
	OnRep_Attributes();
}

void AAFPlayerState::AddKill()
{
	if (!HasAuthority()) return;
	KillCount++;
	OnRep_PlayerScore();
}

void AAFPlayerState::AddDeath()
{
	if (!HasAuthority()) return;
	DeathCount++;
	OnRep_PlayerScore();
}

void AAFPlayerState::SetTeamInfo(uint8 NewTeamID, uint8 NewTeamIndex)
{
	if (!HasAuthority()) return;
	TeamID = NewTeamID;
	TeamIndex = NewTeamIndex;
	OnRep_PlayerInfo();

	ForceNetUpdate();
}

void AAFPlayerState::ResetForRespawn()
{
	if (!HasAuthority()) return;
	OnRep_PlayerInfo();
}

void AAFPlayerState::SetSelectedCharacter_Server(uint8 InId)
{
	if (!HasAuthority()) return;
	SelectedCharacterId = InId;

	// ID에 따라 실제 데이터 테이블의 RowName(Mage, Archer 등)을 직접 매핑
	// (CharacterDisplayNames 배열에 의존하지 않는 방식이 더 안전합니다)
	switch (InId)
	{
	case 0: SelectedCharacterKey = TEXT("Mage"); break;
	case 1: SelectedCharacterKey = TEXT("Archer"); break;
	case 2: SelectedCharacterKey = TEXT("WereWolf"); break;
	default: SelectedCharacterKey = TEXT("Unknown"); break;
	}

	OnRep_PlayerInfo();
}

void AAFPlayerState::SetReady_Server(bool bNewReady)
{
	if (!HasAuthority()) return;
	bReady = bNewReady;
	OnRep_PlayerInfo();
}

void AAFPlayerState::ResetLobbySelection_Server()
{
	if (!HasAuthority()) return;
	SelectedCharacterId = 255;
	SelectedCharacterName = FText::GetEmpty();
	bReady = false;
	OnRep_PlayerInfo();
}

<<<<<<< Updated upstream
void AAFPlayerState::AddMana(float Amount)
=======
FText AAFPlayerState::GetSelectedCharacterName() const
{
	return SelectedCharacterName.IsEmpty() ? FText::FromString(TEXT("Unknown")) : SelectedCharacterName;
}

bool AAFPlayerState::IsDead() const
>>>>>>> Stashed changes
{
	APawn* OwningPawn = GetPawn();
	if (OwningPawn)
	{
		UAFAttributeComponent* AttrComp = OwningPawn->FindComponentByClass<UAFAttributeComponent>();
		if (AttrComp)
		{
			return AttrComp->IsDead();
		}
	}
	return true;
}

<<<<<<< Updated upstream









=======
#pragma endregion
>>>>>>> Stashed changes
