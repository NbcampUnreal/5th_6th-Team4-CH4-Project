#include "Game/AFGameMode.h"
#include "Game/AFGameState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "AFO/Public/Player/AFPlayerState.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Player/AFPlayerController.h"

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

	if (!NewPlayer) return;

	uint8 AssignedTeam = (PlayerTeams.Num() % 2 == 0) ? 0 : 1; // 0:RED, 1:BLUE
	PlayerTeams.Add(NewPlayer, AssignedTeam);

	if (AAFPlayerState* PS = NewPlayer->GetPlayerState<AAFPlayerState>())
	{
		int32 TeamCount = 0;
		for (const auto& Elem : PlayerTeams)
		{
			if (Elem.Value == AssignedTeam)
			{
				TeamCount++;
			}
		}

		PS->SetTeamInfo(AssignedTeam, (uint8)TeamCount);
		PS->SetDead(false);
		PS->SetHealth(PS->GetMaxHealth(), PS->GetMaxHealth());
		PS->SetMana(PS->GetMaxMana(), PS->GetMaxMana());

		UE_LOG(LogTemp, Warning, TEXT("Player Joined: %s Team = %s, Index = %d"),
			*NewPlayer->GetName(),
			AssignedTeam == 0 ? TEXT("RED") : TEXT("BLUE"),
			TeamCount);
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
	if (!HasAuthority() || !VictimController)
	{
		return;
	}

	AAFPlayerState* VictimPS = VictimController->GetPlayerState<AAFPlayerState>();
	if (!VictimPS)
	{
		return;
	}

	if (VictimPS->IsDead())
	{
		return;
	}

	VictimPS->IncrementDeathCount();
	VictimPS->SetDead(true);

	if (KillerController && KillerController != VictimController)
	{
		if (AAFPlayerState* KillerPS = KillerController->GetPlayerState<AAFPlayerState>())
		{
			KillerPS->IncrementKillCount();
		}

		ReportKill(KillerController);
	}

	if (APawn* Pawn = VictimController->GetPawn())
	{
		Pawn->DetachFromControllerPendingDestroy();
		Pawn->Destroy();
	}

	// 리스폰 위젯 띄우기
	if (AAFPlayerController* PC = Cast<AAFPlayerController>(VictimController))
	{
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
	if (!GS)
	{
		UE_LOG(LogTemp, Error, TEXT("AFGameMode::EndRound - GameState is null"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Round End → Red: %d / Blue: %d"),
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