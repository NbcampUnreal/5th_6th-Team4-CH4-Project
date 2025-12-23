#include "Game/AFCharacterSelectGameMode.h"
#include "Game/AFLobbyGameState.h"
#include "Player/AFPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/GameState.h"

AAFCharacterSelectGameMode::AAFCharacterSelectGameMode()
{
	bUseSeamlessTravel = true;
	bStartPlayersAsSpectators = true;

	GameStateClass = AAFLobbyGameState::StaticClass();

	// 기본 4개 더미(나중에 BP에서 DisplayName/Id 바꿔서 쓰면 됨)
	CharacterOptions =
	{
		{0, FText::FromString(TEXT("Character A"))},
		{1, FText::FromString(TEXT("Character B"))},
		{2, FText::FromString(TEXT("Character C"))},
		{3, FText::FromString(TEXT("Character D"))},
	};
}

bool AAFCharacterSelectGameMode::IsCharacterTakenInTeam(uint8 TeamId, uint8 CharacterId, APlayerState* Except) const
{
	if (!GameState) return false;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (PS == Except) continue;

		if (AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS))
		{
			if (AFPS->GetTeamID() == TeamId &&
				AFPS->HasSelectedCharacter() &&
				AFPS->GetSelectedCharacterId() == CharacterId)
			{
				return true;
			}
		}
	}
	return false;
}

bool AAFCharacterSelectGameMode::RequestSelectCharacter(AController* Requester, uint8 CharacterId)
{
	if (!HasAuthority() || !Requester) return false;

	AAFPlayerState* PS = Requester->GetPlayerState<AAFPlayerState>();
	if (!PS) return false;

	// 팀 내 중복 금지
	if (IsCharacterTakenInTeam(PS->GetTeamID(), CharacterId, PS))
	{
		return false;
	}

	PS->SetSelectedCharacter_Server(CharacterId);

	// 캐릭 바꾸면 Ready는 풀어주는 게 안전
	PS->SetReady_Server(false);

	TryStartBattle();
	return true;
}

bool AAFCharacterSelectGameMode::RequestSetReady(AController* Requester, bool bNewReady)
{
	if (!HasAuthority() || !Requester) return false;

	AAFPlayerState* PS = Requester->GetPlayerState<AAFPlayerState>();
	if (!PS) return false;

	if (!PS->HasSelectedCharacter())
	{
		return false;
	}

	PS->SetReady_Server(bNewReady);
	TryStartBattle();
	return true;
}

bool AAFCharacterSelectGameMode::IsAllPickedAndReady() const
{
	if (!GameState) return false;
	if (GameState->PlayerArray.Num() != 4) return false;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS);
		if (!AFPS) return false;

		if (!AFPS->HasSelectedCharacter()) return false;
		if (!AFPS->IsReady()) return false;
	}

	return true;
}

void AAFCharacterSelectGameMode::TryStartBattle()
{
	if (!IsAllPickedAndReady()) return;

	GetWorld()->ServerTravel(*BattleZoneURL);
}
