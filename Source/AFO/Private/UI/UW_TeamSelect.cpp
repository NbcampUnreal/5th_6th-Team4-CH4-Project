#include "UI/UW_TeamSelect.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameState.h"
#include "Controller/AFLobbyPlayerController.h"
#include "Game/AFLobbyGameState.h"
#include "Player/AFPlayerState.h"
#include "TimerManager.h"

void UUW_TeamSelect::NativeConstruct()
{
	Super::NativeConstruct();

	if (BtnToRed)  BtnToRed->OnClicked.AddDynamic(this, &ThisClass::OnClickToRed);
	if (BtnToBlue) BtnToBlue->OnClicked.AddDynamic(this, &ThisClass::OnClickToBlue);
	if (BtnNext)   BtnNext->OnClicked.AddDynamic(this, &ThisClass::OnClickNext);

	TryBindGameState();
	RefreshUI();
}

void UUW_TeamSelect::TryBindGameState()
{
	if (bBound) return;

	UWorld* World = GetWorld();
	if (!World) return;

	AAFLobbyGameState* LGS = World->GetGameState<AAFLobbyGameState>();
	if (!LGS)
	{
		World->GetTimerManager().SetTimer(BindRetryHandle, this, &ThisClass::TryBindGameState, 0.2f, true);
		return;
	}

	LGS->OnCountsChanged.AddDynamic(this, &ThisClass::RefreshUI);
	bBound = true;

	World->GetTimerManager().ClearTimer(BindRetryHandle);

	RefreshUI();
}

void UUW_TeamSelect::OnClickToRed()
{
	if (AAFLobbyPlayerController* PC = GetOwningPlayer<AAFLobbyPlayerController>())
	{
		PC->ServerRequestSetTeam(0);
	}
}

void UUW_TeamSelect::OnClickToBlue()
{
	if (AAFLobbyPlayerController* PC = GetOwningPlayer<AAFLobbyPlayerController>())
	{
		PC->ServerRequestSetTeam(1);
	}
}

void UUW_TeamSelect::OnClickNext()
{
	if (AAFLobbyPlayerController* PC = GetOwningPlayer<AAFLobbyPlayerController>())
	{
		PC->ServerRequestAdvanceToCharacterSelect();
	}
}

void UUW_TeamSelect::RefreshUI()
{
	AAFLobbyGameState* LGS = GetWorld() ? GetWorld()->GetGameState<AAFLobbyGameState>() : nullptr;
	if (!LGS) return;

	if (TxtCounts)
	{
		const FString S = FString::Printf(TEXT("Connected: %d / Red: %d / Blue: %d"),
			LGS->ConnectedPlayers, LGS->RedCount, LGS->BlueCount);

		TxtCounts->SetText(FText::FromString(S));
	}

	if (BtnNext)
	{
		const bool bEnableNext = (LGS->ConnectedPlayers == 4 && LGS->RedCount == 2 && LGS->BlueCount == 2);
		BtnNext->SetIsEnabled(bEnableNext);
	}

	RebuildTeamLists();

	RebuildPlayerList();
}

FString UUW_TeamSelect::BuildTeamList(uint8 TeamId) const
{
	AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if (!GS) return TEXT("- empty");

	FString Out;
	int32 Num = 0;

	for (APlayerState* PS : GS->PlayerArray)
	{
		AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS);
		if (!AFPS) continue;

		if (AFPS->GetTeamID() == TeamId)
		{
			Num++;
			Out += FString::Printf(TEXT("%d) %s\n"),
				(int32)AFPS->GetTeamIndex(), *AFPS->GetPlayerName());
		}
	}

	Out.TrimEndInline();
	return (Num == 0) ? TEXT("- empty") : Out;
}

void UUW_TeamSelect::RebuildTeamLists()
{
	AAFLobbyGameState* LGS = GetWorld() ? GetWorld()->GetGameState<AAFLobbyGameState>() : nullptr;
	if (!LGS) return;

	if (Txt_RedCount)
	{
		Txt_RedCount->SetText(FText::FromString(FString::Printf(TEXT("Red: %d/2"), LGS->RedCount)));
	}
	if (Txt_BlueCount)
	{
		Txt_BlueCount->SetText(FText::FromString(FString::Printf(TEXT("Blue: %d/2"), LGS->BlueCount)));
	}

	if (Txt_RedList)
	{
		Txt_RedList->SetText(FText::FromString(BuildTeamList(0)));
	}
	if (Txt_BlueList)
	{
		Txt_BlueList->SetText(FText::FromString(BuildTeamList(1)));
	}
}

void UUW_TeamSelect::RebuildPlayerList()
{
	if (!VBPlayerList) return;

	VBPlayerList->ClearChildren();

	AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if (!GS) return;

	for (APlayerState* PS : GS->PlayerArray)
	{
		AAFPlayerState* AFPS = Cast<AAFPlayerState>(PS);
		if (!AFPS) continue;

		const TCHAR* TeamStr = (AFPS->GetTeamID() == 0) ? TEXT("RED") : TEXT("BLUE");

		const FString Line = FString::Printf(TEXT("%s | Team:%s | Index:%d"),
			*AFPS->GetPlayerName(), TeamStr, (int32)AFPS->GetTeamIndex());

		UTextBlock* NewText = NewObject<UTextBlock>(this);
		NewText->SetText(FText::FromString(Line));
		VBPlayerList->AddChild(NewText);
	}
}
