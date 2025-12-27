#include "Game/AFTeamSelectGameMode.h"
#include "Game/AFLobbyGameState.h"
#include "Player/AFPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/GameState.h"
#include "Controller/AFLobbyPlayerController.h"

AAFTeamSelectGameMode::AAFTeamSelectGameMode()
{
	bUseSeamlessTravel = true;
	bStartPlayersAsSpectators = true;

	GameStateClass = AAFLobbyGameState::StaticClass();
	PlayerStateClass = AAFPlayerState::StaticClass();

	PlayerControllerClass = AAFLobbyPlayerController::StaticClass();
}

void AAFTeamSelectGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// 접속 즉시 자동배정(원하면 이 부분 제거하고 “미배정 팀”으로 시작해도 됨)
	//const uint8 AutoTeam = (GetTeamCount(0) <= GetTeamCount(1)) ? 0 : 1;
	//RequestSetTeam(NewPlayer, AutoTeam);

	UpdateLobbyCounts();
}

void AAFTeamSelectGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	RebuildTeamIndices();
	UpdateLobbyCounts();
}

int32 AAFTeamSelectGameMode::GetTeamCount(uint8 TeamId) const
{
	if (!GameState) return 0;

	int32 Count = 0;
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS))
		{
			if (AFPS->GetTeamID() == TeamId)
			{
				Count++;
			}
		}
	}
	return Count;
}

bool AAFTeamSelectGameMode::RequestSetTeam(AController* Requester, uint8 NewTeamId)
{
	if (!HasAuthority() || !Requester) return false;
	if (NewTeamId > 1) return false;

	AAFPlayerState* PS = Requester->GetPlayerState<AAFPlayerState>();
	if (!PS) return false;

	if (PS->GetTeamID() == NewTeamId)
	{
		return true;
	}

	if (GetTeamCount(NewTeamId) >= 2)
	{
		return false;
	}

	PS->SetTeamInfo(NewTeamId, 0);
	PS->ResetLobbySelection_Server();
	PS->ForceNetUpdate();
	RebuildTeamIndices();
	UpdateLobbyCounts();
	return true;
}

void AAFTeamSelectGameMode::RebuildTeamIndices()
{
	if (!GameState) return;

	uint8 RedIndex = 1;
	uint8 BlueIndex = 1;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS))
		{
			if (AFPS->GetTeamID() == 0)
			{
				AFPS->SetTeamInfo(0, RedIndex++);
			}
			else
			{
				AFPS->SetTeamInfo(1, BlueIndex++);
			}

			AFPS->ForceNetUpdate();
		}
	}
}

void AAFTeamSelectGameMode::UpdateLobbyCounts()
{
	AAFLobbyGameState* LGS = GetGameState<AAFLobbyGameState>();
	if (!LGS || !GameState) return;

	LGS->ConnectedPlayers = GameState->PlayerArray.Num();
	LGS->RedCount = GetTeamCount(0);
	LGS->BlueCount = GetTeamCount(1);
	LGS->ForceNetUpdate();
	LGS->OnRep_Counts();
}

bool AAFTeamSelectGameMode::CanAdvanceToCharacterSelect() const
{
	if (!GameState) return false;

	const int32 Total = GameState->PlayerArray.Num();
	const int32 Red = GetTeamCount(0);
	const int32 Blue = GetTeamCount(1);
	const bool bEnoughPlayers = (Total >= 2);
	const bool bHasBothTeams = (Red > 0 && Blue > 0);
	const bool bNoOverflowTeam = (Red <= 2 && Blue <= 2);

	return bEnoughPlayers && bHasBothTeams && bNoOverflowTeam;
}

bool AAFTeamSelectGameMode::AdvanceToCharacterSelect()
{
	if (!CanAdvanceToCharacterSelect())
	{
		return false;
	}

	if (!GetWorld()) return false;

	GetWorld()->ServerTravel(CharacterSelectMapURL, true);
	return true;
}