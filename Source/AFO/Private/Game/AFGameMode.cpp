#include "Game/AFGameMode.h"
#include "Game/AFGameState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AAFGameMode::AAFGameMode()
{
	GameStateClass = AAFGameState::StaticClass();
	RoundDuration = 60;
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

	StartRound();
}

void AAFGameMode::StartRound()
{
	AAFGameState* GS = GetAFGameState();
	if (!GS)
	{
		UE_LOG(LogTemp, Error, TEXT("AFGameMode::StartRound - GameState is null"));
		return;
	}

	GS->SetRemainingTime(RoundDuration);
	GS->StartGameTimer();

	UE_LOG(LogTemp, Warning, TEXT("Round Start, Duration: %d"), RoundDuration);
}

void AAFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 CurrentPlayerCount = PlayerTeams.Num();

	uint8 AssignedTeam = (CurrentPlayerCount % 2 == 0) ? 0 : 1;
	PlayerTeams.Add(NewPlayer, AssignedTeam);

	UE_LOG(LogTemp, Warning, TEXT("Player Joined: %s Team = %s"),
		*NewPlayer->GetName(),
		AssignedTeam == 0 ? TEXT("RED") : TEXT("BLUE"));
}

void AAFGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		PlayerTeams.Remove(PC);
		UE_LOG(LogTemp, Warning, TEXT("Player Left Game: %s"), *PC->GetName());
	}
}

void AAFGameMode::ReportKill(APlayerController* Killer, APlayerController* Victim)
{
	AAFGameState* GS = GetAFGameState();
	if (!GS || !Killer || !PlayerTeams.Contains(Killer))
		return;

	uint8 KillerTeam = PlayerTeams[Killer];

	if (KillerTeam == 0)
	{
		GS->TeamRedKillScore++;
	}
	else
	{
		GS->TeamBlueKillScore++;
	}

	UE_LOG(LogTemp, Warning, TEXT("Kill ¡æ Red: %d / Blue: %d"),
		GS->TeamRedKillScore,
		GS->TeamBlueKillScore);
}

void AAFGameMode::EndRound()
{
	AAFGameState* GS = GetAFGameState();
	if (!GS)
	{
		UE_LOG(LogTemp, Error, TEXT("AFGameMode::EndRound - GameState is null"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Round End ¡æ Red: %d / Blue: %d"),
		GS->TeamRedKillScore,
		GS->TeamBlueKillScore);

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

	GS->SetGamePhase(EAFGamePhase::EAF_GameOver);

	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(TEXT("TitleMenu"));
	}
}